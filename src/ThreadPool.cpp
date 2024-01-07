#include "ThreadPool.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace threading
{
	ThreadPool::Worker::Worker(ThreadPool* threadPool) :
		thread(&ThreadPool::workerThread, threadPool, this),
		state(threadState::waiting),
		running(true)
	{

	}

	ThreadPool::Worker::Worker(const Worker& other)
	{
		(*this) = other;
	}

	ThreadPool::Worker& ThreadPool::Worker::operator = (const Worker& other)
	{
		thread = move(const_cast<Worker&>(other).thread);

		return *this;
	}

	void ThreadPool::Worker::join()
	{
		thread.join();
	}

	void ThreadPool::Worker::detach()
	{
		thread.detach();
	}

	void ThreadPool::workerThread(Worker* worker)
	{
		while (worker->running)
		{
			{
				unique_lock<mutex> lock(worker->workerMutex);

				hasTask.wait
				(
					lock,
					[this, worker]() 
					{
						if (!worker->running)
						{
							return true;
						}

						if (tasks.size())
						{
							return true;
						}

						return false;
					}
				);

				if (!worker->running)
				{
					break;
				}

				worker->task = move(tasks.pop());
			}

			worker->state = threadState::running;

			worker->task->execute();

			unique_lock<mutex> threadStateLock(worker->stateMutex);

			worker->task.reset();

			worker->state = threadState::waiting;
		}

		if (worker->onEnd)
		{
			worker->onEnd(worker);
		}
	}

	unique_ptr<Future> ThreadPool::addTask(unique_ptr<BaseTask>&& task)
	{
		task->taskPromise = task->createTaskPromise();

		unique_ptr<Future> result = task->getFuture();

		tasks.push(move(task));

		hasTask.notify_one();

		return result;
	}

	ThreadPool::ThreadPool(size_t threadsCount)
	{
		this->reinit(threadsCount);
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

	void ThreadPool::reinit(bool wait, size_t threadsCount)
	{
		this->shutdown(wait);

		for (size_t i = 0; i < threadsCount; i++)
		{
			workers.push_back(new Worker(this));
		}
	}

	bool ThreadPool::resize(size_t threadsCount)
	{
		if (threadsCount == workers.size())
		{
			return false;
		}
		else if (threadsCount > workers.size())
		{
			for (size_t i = workers.size(); i < threadsCount; i++)
			{
				workers.push_back(new Worker(this));
			}

			return true;
		}

		this->reinit(false, threadsCount);

		return true;
	}

	void ThreadPool::shutdown(bool wait)
	{
		ranges::for_each
		(
			workers,
			[this, wait](Worker* worker)
			{
				worker->running = false;

				if (wait)
				{
					hasTask.notify_all();

					worker->join();

					delete worker;
				}
				else
				{
					worker->onEnd = [](Worker* worker)
					{
						delete worker;
					};

					worker->detach();

					hasTask.notify_all();
				}
			}
		);

		workers.clear();
	}

	bool ThreadPool::isAnyTaskRunning() const
	{
		return ranges::any_of(workers, [](Worker* worker) { return worker->state == threadState::running; });
	}

	ThreadPool::threadState ThreadPool::getThreadState(size_t threadIndex) const
	{
		return workers.at(threadIndex)->state;
	}

	float ThreadPool::getThreadProgress(size_t threadIndex) const
	{
		const Worker* worker = workers.at(threadIndex);
		unique_lock<mutex> threadProgressLock(worker->stateMutex);

		return worker->task ? worker->task->getProgress() : -1.0f;
	}

	size_t ThreadPool::getThreadsCount() const
	{
		return static_cast<size_t>(workers.size());
	}

	size_t ThreadPool::size() const
	{
		return static_cast<size_t>(workers.size());
	}

	ThreadPool::~ThreadPool()
	{
		this->shutdown();
	}
}
