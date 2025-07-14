#include "ThreadPool.h"

#include <algorithm>
#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono_literals;

namespace threading
{
	ThreadPool::Worker::Worker(ThreadPool* threadPool) :
		thread(&ThreadPool::workerThread, threadPool, this),
		state(ThreadState::waiting),
		running(true),
		deleteSelf(false)
	{

	}

	void ThreadPool::Worker::join()
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	void ThreadPool::Worker::detach()
	{
		thread.detach();
	}

	void ThreadPool::workerThread(Worker* worker)
	{
		while (worker->running)
		{
			hasTask.acquire();

			worker->state = ThreadState::running;

			if (optional<unique_ptr<BaseTask>> task = tasks.pop())
			{
				worker->task = move(*task);
				worker->task->execute();
				worker->task.reset();
			}

			worker->state = ThreadState::waiting;
		}

		if (worker->deleteSelf)
		{
			delete worker;
		}
	}

	unique_ptr<Future> ThreadPool::addTask(unique_ptr<BaseTask>&& task)
	{
		task->taskPromise = task->createTaskPromise();

		unique_ptr<Future> result = task->getFuture();

		tasks.push(move(task));

		hasTask.release();

		return result;
	}

	string ThreadPool::getVersion()
	{
		string version = "1.6.1";

		return version;
	}

	ThreadPool::ThreadPool(size_t threadsCount) :
		hasTask(0)
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

	void ThreadPool::reinit(bool wait, size_t threadsCount)
	{
		this->shutdown(wait);

		workers.reserve(threadsCount);

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
		for (Worker* worker : workers)
		{
			if (!wait)
			{
				worker->detach();

				worker->deleteSelf = true;
			}

			worker->running = false;
		}

		if (wait)
		{
			while (tasks.size())
			{
				std::cout << "Wait until all tasks will be finished" << std::endl;

				this_thread::sleep_for(1s);
			}

			hasTask.release(workers.size());

			for (Worker* worker : workers)
			{
				worker->join();

				delete worker;
			}
		}
		else
		{
			tasks.clear();

			hasTask.release(workers.size());
		}

		workers.clear();
	}

	bool ThreadPool::isAnyTaskRunning() const
	{
		return ranges::any_of(workers, [](Worker* worker) { return worker->state == ThreadState::running; });
	}

	ThreadPool::ThreadState ThreadPool::getThreadState(size_t threadIndex) const
	{
		return workers.at(threadIndex)->state;
	}

	float ThreadPool::getThreadProgress(size_t threadIndex) const
	{
		const Worker* worker = workers.at(threadIndex);

		return worker->task ? worker->task->getProgress() : -1.0f;
	}

	size_t ThreadPool::getThreadsCount() const
	{
		return this->size();
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
