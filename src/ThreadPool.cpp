#include "ThreadPool.h"

#include <algorithm>
#include <chrono>

namespace threading
{
	void ThreadPool::Worker::workerThread(std::shared_ptr<utility::ConcurrentQueue<std::unique_ptr<BaseTask>>> tasks, std::shared_ptr<std::counting_semaphore<(std::numeric_limits<int32_t>::max)()>> hasTask)
	{
		id = std::this_thread::get_id();

		while (running)
		{
			hasTask->acquire();

			state = ThreadState::running;

			if (std::optional<std::unique_ptr<BaseTask>> newTask = tasks->pop())
			{
				task = std::move(*newTask);
				task->execute();
				task.reset();
			}

			state = ThreadState::waiting;
		}

		if (deleteSelf)
		{
			delete this;
		}
	}

	ThreadPool::Worker::Worker(ThreadPool* threadPool) :
		state(ThreadState::waiting),
		running(true),
		deleteSelf(false),
		thread(&Worker::workerThread, this, threadPool->tasks, threadPool->hasTask)
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

	ThreadPool::Worker::~Worker()
	{
		this->join();
	}

	std::unique_ptr<Future> ThreadPool::addTask(std::unique_ptr<BaseTask>&& task)
	{
		task->taskPromise = task->createTaskPromise();

		std::unique_ptr<Future> result = task->getFuture();

		tasks->push(move(task));

		hasTask->release();

		return result;
	}

	std::string ThreadPool::getVersion()
	{
		std::string version = "1.8.0";

		return version;
	}

	ThreadPool::ThreadPool(size_t threadsCount)
	{
		this->reinit(true, threadsCount);
	}

	std::unique_ptr<Future> ThreadPool::addTask(const std::function<void()>& task, const std::function<void()>& callback)
	{
		return this->addTask
		(
			std::make_unique<FunctionWrapperTask<void>>(task, callback)
		);
	}

	std::unique_ptr<Future> ThreadPool::addTask(const std::function<void()>& task, std::function<void()>&& callback)
	{
		return this->addTask
		(
			std::make_unique<FunctionWrapperTask<void>>(task, move(callback))
		);
	}

	std::unique_ptr<Future> ThreadPool::addTask(std::function<void()>&& task, const std::function<void()>& callback)
	{
		return this->addTask
		(
			std::make_unique<FunctionWrapperTask<void>>(move(task), callback)
		);
	}

	std::unique_ptr<Future> ThreadPool::addTask(std::function<void()>&& task, std::function<void()>&& callback)
	{
		return this->addTask
		(
			std::make_unique<FunctionWrapperTask<void>>(move(task), move(callback))
		);
	}

	void ThreadPool::reinit(bool wait, size_t threadsCount)
	{
		if (workers.size())
		{
			this->shutdown(wait);
		}

		hasTask = std::make_shared<std::counting_semaphore<(std::numeric_limits<int32_t>::max)()>>(0);
		tasks = std::make_shared<utility::ConcurrentQueue<std::unique_ptr<BaseTask>>>();

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
		using namespace std::chrono_literals;

		if (wait)
		{
			while (tasks->size())
			{
				std::this_thread::sleep_for(1s);
			}

			for (Worker* worker : workers)
			{
				worker->running = false;
			}

			hasTask->release(workers.size());

			for (Worker* worker : workers)
			{
				worker->join();

				delete worker;
			}
		}
		else
		{
			for (Worker* worker : workers)
			{
				worker->detach();

				worker->deleteSelf = true;
				worker->running = false;
			}

			tasks->clear();

			hasTask->release(workers.size());
		}

		workers.clear();
	}

	bool ThreadPool::isAnyTaskRunning() const
	{
		return std::ranges::any_of(workers, [](Worker* worker) { return worker->state == ThreadState::running; });
	}

	ThreadPool::ThreadState ThreadPool::getThreadState(size_t threadIndex) const
	{
		return workers.at(threadIndex)->state;
	}

	float ThreadPool::getThreadProgress(size_t threadIndex) const
	{
		const Worker* worker = workers.at(threadIndex);
		std::shared_ptr<BaseTask> task = worker->task;

		return task ? task->getProgress() : -1.0f;
	}

	std::thread::id ThreadPool::getThreadId(size_t threadIndex) const
	{
		return workers.at(threadIndex)->id;
	}

	size_t ThreadPool::getThreadsCount() const
	{
		return this->size();
	}

	size_t ThreadPool::getQueuedTasks() const
	{
		return tasks->size();
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
