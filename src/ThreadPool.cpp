#include "ThreadPool.h"

#include <algorithm>

using namespace std;

namespace threading
{
	void ThreadPool::mainWorkerThread(size_t threadIndex)
	{
		threadPoolTask currentTask;

		while (true)
		{
			{
				unique_lock<mutex> lock(tasksMutex);

				hasTask.wait(lock, [this]() { return tasks.size() || terminate; });

				if (terminate)
				{
					return;
				}
				
				currentTask = move(tasks.front());

				tasks.pop();
			}

			threadsState[threadIndex] = true;

			currentTask.task();

			if (currentTask.callback)
			{
				currentTask.callback();
			}

			threadsState[threadIndex] = false;
		}
	}

	ThreadPool::ThreadPool(uint32_t threadsCount) :
		threads(threadsCount),
		threadsCount(threadsCount),
		terminate(false)
	{
		this->reinit();
	}

	void ThreadPool::addTask(const function<void()>& task, const function<void()>& callback)
	{
		threadPoolTask functions;

		functions.task = task;
		functions.callback = callback;

		{
			lock_guard<mutex> lock(tasksMutex);

			tasks.push(move(functions));
		}

		hasTask.notify_one();
	}

	void ThreadPool::addTask(function<void()>&& task, const function<void()>& callback)
	{
		threadPoolTask functions;

		functions.task = move(task);
		functions.callback = callback;

		{
			lock_guard<mutex> lock(tasksMutex);

			tasks.push(move(functions));
		}

		hasTask.notify_one();
	}

	void ThreadPool::reinit(bool changeThreadsCount, uint32_t threadsCount)
	{
		terminate = false;

		if (changeThreadsCount && this->getThreadsCount() != threadsCount)
		{
			this->threadsCount = threadsCount;

			threads.clear();
			threadsState.clear();

			threads.resize(threadsCount);
			threadsState.resize(threadsCount);
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			threads[i] = make_unique<thread>(&ThreadPool::mainWorkerThread, this, i);

			threadsState[i] = false;
		}
	}

	void ThreadPool::shutdown()
	{
		terminate = true;

		hasTask.notify_all();

		for (unique_ptr<thread>& i : threads)
		{
			if (i)
			{
				i->join();

				i.reset();
			}
		}
	}

	bool ThreadPool::isAnyTaskRunning() const
	{
		return ranges::any_of(threadsState, [](bool state) { return state; });
	}

	uint32_t ThreadPool::getThreadsCount() const
	{
		return threadsCount;
	}

	ThreadPool::~ThreadPool()
	{
		this->shutdown();
	}
}
