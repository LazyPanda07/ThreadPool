#include "ThreadPool.h"

using namespace std;

namespace threading
{
	void ThreadPool::mainWorkerThread()
	{
		function<void()> currentTask;

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

			currentTask();
		}
	}

	ThreadPool::ThreadPool(uint32_t threadCount) :
		threads(threadCount),
		terminate(false)
	{
		this->init();
	}

	void ThreadPool::addTask(const std::function<void()>& task)
	{
		{
			unique_lock<mutex> lock(tasksMutex);

			tasks.push(task);
		}

		hasTask.notify_one();
	}

	void ThreadPool::addTask(std::function<void()>&& task) noexcept
	{
		{
			unique_lock<mutex> lock(tasksMutex);

			tasks.push(move(task));
		}

		hasTask.notify_one();
	}

	void ThreadPool::init()
	{
		terminate = false;

		for (unique_ptr<thread>& i : threads)
		{
			i = make_unique<thread>(&ThreadPool::mainWorkerThread, this);
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

	ThreadPool::~ThreadPool()
	{
		this->shutdown();
	}
}
