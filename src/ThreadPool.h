#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <vector>
#include <functional>

#include "Tasks/FunctionWrapperTask.h"

namespace threading
{
	/// @brief ThreadPool
	class THREAD_POOL_API ThreadPool final
	{
	public:
		enum class threadState
		{
			running,
			waiting
		};

	private:
		std::queue<std::unique_ptr<BaseTask>> tasks;
		std::condition_variable hasTask;
		std::mutex tasksMutex;
		std::vector<std::jthread> threads;
		std::vector<threadState> threadStates;
		bool terminate;

	private:
		void workerThread(size_t threadIndex);

		std::unique_ptr<Future> addTask(std::unique_ptr<BaseTask>&& task);

	public:
		/// @brief Construct ThreadPool
		/// @param threadCount Number of threads in ThreadPool(default is max threads for current hardware)
		ThreadPool(uint32_t threadsCount = std::thread::hardware_concurrency());

		/// @brief Add new task to thread pool
		std::unique_ptr<Future> addTask(const std::function<void()>& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to thread pool
		std::unique_ptr<Future> addTask(const std::function<void()>& task, std::function<void()>&& callback);

		/// @brief Add new task to thread pool
		std::unique_ptr<Future> addTask(std::function<void()>&& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to thread pool
		std::unique_ptr<Future> addTask(std::function<void()>&& task, std::function<void()>&& callback);

		/// @brief Add new task to thread pool
		template<typename R, typename... ArgsT, typename... Args>
		std::unique_ptr<Future> addTask(const std::function<R(ArgsT...)>& task, const std::function<void()>& callback, Args&&... args);

		/// @brief Add new task to thread pool
		template<typename R, typename... ArgsT, typename... Args>
		std::unique_ptr<Future> addTask(const std::function<R(ArgsT...)>& task, std::function<void()>&& callback, Args&&... args);

		/// @brief Reinitialize thread pool
		/// @param changeThreadsCount Changes current thread pool size
		/// @param threadsCount New thread pool size
		void reinit(bool changeThreadsCount = false, uint32_t threadsCount = std::thread::hardware_concurrency());

		/// @brief Stop ThreadPool with joining all threads
		void shutdown();

		/// @brief Check is thread pool has task that running in some thread
		/// @return Returns true if thread pool has task
		bool isAnyTaskRunning() const;

		/// @brief Check specific thread state
		/// @param threadIndex Index of thread between 0 and threadsCount
		/// @return Thread state
		threadState getThreadState(uint32_t threadIndex) const;

		/// @brief Getter for threadsCount
		/// @return Current count of threads in thread pool
		uint32_t getThreadsCount() const;

		~ThreadPool();
	};

	template<typename R, typename... ArgsT, typename... Args>
	std::unique_ptr<Future> ThreadPool::addTask(const std::function<R(ArgsT...)>& task, const std::function<void()>& callback, Args&&... args)
	{
		return this->addTask
		(
			std::make_unique<FunctionWrapperTask<R>>(std::bind(task, std::forward<Args>(args)...), callback)
		);
	}

	template<typename R, typename... ArgsT, typename... Args>
	std::unique_ptr<Future> ThreadPool::addTask(const std::function<R(ArgsT...)>& task, std::function<void()>&& callback, Args&&... args)
	{
		return this->addTask
		(
			std::make_unique<FunctionWrapperTask<R>>(std::bind(task, std::forward<Args>(args)...), std::move(callback))
		);
	}
}
