#include "ThreadPool.h"

using namespace std;

namespace threading
{
	void ThreadPool::mainWorkerThread()
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

			currentTask.task();

			if (currentTask.callback)
			{
				currentTask.callback();
			}
		}
	}

	ThreadPool::ThreadPool(uint32_t threadCount) :
		threads(threadCount),
		terminate(false)
	{
		this->init();
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
