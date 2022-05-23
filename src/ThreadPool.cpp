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

			threadStates[threadIndex] = threadState::running;

			currentTask.task();

			if (currentTask.callback)
			{
				currentTask.callback();
			}

			threadStates[threadIndex] = threadState::waiting;
		}
	}

	ThreadPool::ThreadPool(uint32_t threadsCount) :
		terminate(false)
	{
		this->reinit(true, threadsCount);
	}

	void ThreadPool::addTask(const function<void()>& task, const function<void()>& callback)
	{
		threadPoolTask functions;

		functions.task = task;
		functions.callback = callback;

		{
			unique_lock<mutex> lock(tasksMutex);

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
			unique_lock<mutex> lock(tasksMutex);

			tasks.push(move(functions));
		}

		hasTask.notify_one();
	}

	void ThreadPool::reinit(bool changeThreadsCount, uint32_t threadsCount)
	{
		this->shutdown();

		terminate = false;

		if (changeThreadsCount && this->getThreadsCount() != threadsCount)
		{
			threads.clear();
			threadStates.clear();

			threads.reserve(threadsCount);
			threadStates.resize(threadsCount);
		}

		for (size_t i = 0; i < threadStates.size(); i++)
		{
			threads.emplace_back(&ThreadPool::mainWorkerThread, this, i);

			threadStates[i] = threadState::waiting;
		}
	}

	void ThreadPool::shutdown()
	{
		{
			unique_lock<mutex> lock(tasksMutex);

			terminate = true;
		}

		hasTask.notify_all();

		threads.clear();

		threadStates.clear();
	}

	bool ThreadPool::isAnyTaskRunning() const
	{
		return ranges::any_of(threadStates, [](threadState state) { return state == threadState::running; });
	}

	ThreadPool::threadState ThreadPool::getThreadState(uint32_t threadIndex) const
	{
		return threadStates[static_cast<uint32_t>(threadIndex)];
	}

	uint32_t ThreadPool::getThreadsCount() const
	{
		return static_cast<uint32_t>(threads.size());
	}

	ThreadPool::~ThreadPool()
	{
		this->shutdown();
	}
}
