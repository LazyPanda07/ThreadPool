#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <functional>
#include <vector>
#include <memory>

namespace threading
{
	class ThreadPool final
	{
	private:
		std::queue<std::function<void()>> tasks;
		std::condition_variable hasTask;
		std::mutex tasksMutex;
		std::vector<std::unique_ptr<std::thread>> threads;
		bool terminate;

	private:
		void mainWorkerThread();

	public:
		/// @brief Construct ThreadPool, then calling ThreadPool::init()
		/// @param threadCount Number of threads in ThreadPool(default is max threads for current hardware)
		ThreadPool(uint32_t threadCount = std::thread::hardware_concurrency());

		/// @brief Add new task to ThreadPool
		void addTask(const std::function<void()>& task);

		/// @brief Add new task to ThreadPool
		void addTask(std::function<void()>&& task) noexcept;
		
		/// @brief Initialize ThreadPool with current size of threads
		void init();

		/// @brief Stop ThreadPool with joining all threads
		void shutdown();

		~ThreadPool();
	};
}
