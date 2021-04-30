#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <functional>
#include <array>
#include <memory>

namespace threading
{
	/// @brief Singleton version of ThreadPool
	template<uint16_t threadCount>
	class SingletonThreadPool final
	{
	private:
		std::queue<std::function<void()>> tasks;
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
		void addTask(const std::function<void()>& task);

		/// @brief Add new task to SingletonThreadPool
		void addTask(std::function<void()>&& task) noexcept;
	};

	template<uint16_t threadCount>
	SingletonThreadPool<threadCount>::SingletonThreadPool() :
		terminate(false)
	{
		for (std::unique_ptr<std::thread>& i : threads)
		{
			i = std::make_unique<std::thread>(&SingletonThreadPool::mainWorkerThread, this);
		}
	}

	template<uint16_t threadCount>
	SingletonThreadPool<threadCount>::~SingletonThreadPool()
	{
		terminate = true;

		for (std::unique_ptr<std::thread>& i : threads)
		{
			i->join();
		}
	}

	template<uint16_t threadCount>
	void SingletonThreadPool<threadCount>::mainWorkerThread()
	{
		std::function<void()> currentTask;

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

			currentTask();
		}
	}

	template<uint16_t threadCount>
	SingletonThreadPool<threadCount>& SingletonThreadPool<threadCount>::get()
	{
		static SingletonThreadPool<threadCount> instance;

		return instance;
	}

	template<uint16_t threadCount>
	void SingletonThreadPool<threadCount>::addTask(const std::function<void()>& task)
	{
		{
			std::unique_lock<std::mutex> lock(tasksMutex);

			tasks.push(move(task));
		}

		hasTask.notify_one();
	}

	template<uint16_t threadCount>
	void SingletonThreadPool<threadCount>::addTask(std::function<void()>&& task) noexcept
	{
		{
			std::unique_lock<std::mutex> lock(tasksMutex);

			tasks.push(move(std::move(task)));
		}

		hasTask.notify_one();
	}
}
