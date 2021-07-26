#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <vector>
#include <memory>

#include "threadPoolTask.h"

#ifdef THREAD_POOL_DLL
#define THREAD_POOL_API __declspec(dllexport)

#pragma warning(disable: 4251)
#else
#define THREAD_POOL_API
#endif // THREAD_POOL_DLL

namespace threading
{
	/// @brief ThreadPool
	class THREAD_POOL_API ThreadPool final
	{
	private:
		std::queue<threadPoolTask> tasks;
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
		void addTask(const std::function<void()>& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to ThreadPool
		void addTask(std::function<void()>&& task, const std::function<void()>& callback = nullptr);
		
		/// @brief Initialize ThreadPool with current size of threads
		void init();

		/// @brief Stop ThreadPool with joining all threads
		void shutdown();

		~ThreadPool();
	};
}
