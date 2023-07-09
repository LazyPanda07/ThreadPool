#include "ThreadPool.h"

#include <algorithm>

using namespace std;

namespace threading
{
	void ThreadPool::workerThread(size_t threadIndex)
	{
		unique_ptr<BaseTask> currentTask;

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

			currentTask->execute();

			threadStates[threadIndex] = threadState::waiting;
		}
	}

	unique_ptr<Future> ThreadPool::addTask(unique_ptr<BaseTask>&& task)
	{
		task->taskPromise = task->createTaskPromise();

		unique_ptr<Future> result = task->getFuture();

		{
			unique_lock<mutex> lock(tasksMutex);

			tasks.push(move(task));
		}

		hasTask.notify_one();

		return result;
	}

	ThreadPool::ThreadPool(uint32_t threadsCount) :
		terminate(false)
	{
		this->reinit(true, threadsCount);
	}

	unique_ptr<Future> ThreadPool::addTask(const function<void()>& task, const function<void()>& callback)
	{
		return this->addTask
		(
			make_unique<FunctionWrapperTask<void>>(task, callback)
		);
	}

	unique_ptr<Future> ThreadPool::addTask(const function<void()>& task, function<void()>&& callback)
	{
		return this->addTask
		(
			make_unique<FunctionWrapperTask<void>>(task, move(callback))
		);
	}

	unique_ptr<Future> ThreadPool::addTask(function<void()>&& task, const function<void()>& callback)
	{
		return this->addTask
		(
			make_unique<FunctionWrapperTask<void>>(move(task), callback)
		);
	}

	unique_ptr<Future> ThreadPool::addTask(function<void()>&& task, function<void()>&& callback)
	{
		return this->addTask
		(
			make_unique<FunctionWrapperTask<void>>(move(task), move(callback))
		);
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
			threads.emplace_back(&ThreadPool::workerThread, this, i);

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
