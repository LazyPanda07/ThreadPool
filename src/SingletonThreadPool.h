#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <functional>
#include <array>
#include <memory>

#include "threadPoolTask.h"

#ifdef THREAD_POOL_DLL
#define THREAD_POOL_API __declspec(dllexport)
#else
#define THREAD_POOL_API
#endif // THREAD_POOL_DLL

namespace threading
{
	/// @brief Singleton version of ThreadPool
	template<uint32_t threadCount>
	class THREAD_POOL_API SingletonThreadPool final
	{
	private:
		std::queue<threadPoolTask> tasks;
		std::condition_variable hasTask;
		std::mutex tasksMutex;
		std::array<std::unique_ptr<std::thread>, threadCount> threads;
		bool terminate;

	private:
		SingletonThreadPool();

		~SingletonThreadPool();

	private:
		void mainWorkerThread();

	public:
		/// @brief Singleton get instance method
		/// @return Instance for current template argument
		static SingletonThreadPool& get();

		/// @brief Add new task to SingletonThreadPool
		void addTask(const std::function<void()>& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to SingletonThreadPool
		void addTask(std::function<void()>&& task, const std::function<void()>& callback = nullptr);
	};

	template<uint32_t threadCount>
	SingletonThreadPool<threadCount>::SingletonThreadPool() :
		terminate(false)
	{
		for (std::unique_ptr<std::thread>& i : threads)
		{
			i = std::make_unique<std::thread>(&SingletonThreadPool::mainWorkerThread, this);
		}
	}

	template<uint32_t threadCount>
	SingletonThreadPool<threadCount>::~SingletonThreadPool()
	{
		terminate = true;

		for (std::unique_ptr<std::thread>& i : threads)
		{
			i->join();
		}
	}

	template<uint32_t threadCount>
	void SingletonThreadPool<threadCount>::mainWorkerThread()
	{
		threadPoolTask currentTask;

		while (true)
		{
			{
				std::unique_lock<std::mutex> lock(tasksMutex);

				hasTask.wait(lock, [this]() { return tasks.size() || terminate; });

				if (terminate)
				{
					return;
				}

				currentTask = std::move(tasks.front());

				tasks.pop();
			}

			currentTask.task();

			if (currentTask.callback)
			{
				currentTask.callback();
			}
		}
	}

	template<uint32_t threadCount>
	SingletonThreadPool<threadCount>& SingletonThreadPool<threadCount>::get()
	{
		static SingletonThreadPool<threadCount> instance;

		return instance;
	}

	template<uint32_t threadCount>
	void SingletonThreadPool<threadCount>::addTask(const std::function<void()>& task, const std::function<void()>& callback)
	{
		threadPoolTask functions;

		functions.task = task;
		functions.callback = callback;

		{
			std::unique_lock<std::mutex> lock(tasksMutex);

			tasks.push(std::move(functions));
		}

		hasTask.notify_one();
	}

	template<uint32_t threadCount>
	void SingletonThreadPool<threadCount>::addTask(std::function<void()>&& task, const std::function<void()>& callback)
	{
		threadPoolTask functions;

		functions.task = std::move(task);
		functions.callback = callback;

		{
			std::unique_lock<std::mutex> lock(tasksMutex);

			tasks.push(std::move(functions));
		}

		hasTask.notify_one();
	}
}
