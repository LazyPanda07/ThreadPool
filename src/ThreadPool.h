#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <functional>
#include <concepts>

#include "Tasks/FunctionWrapperTask.h"
#include "Utility/ConcurrentQueue.h"

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
		struct Worker
		{
		private:
			std::thread thread;

		public:
			std::unique_ptr<BaseTask> task;
			std::atomic<threadState> state;
			std::atomic_bool running;
			bool deleteSelf;

		public:
			Worker(ThreadPool* threadPool);

			void join();

			void detach();

			~Worker() = default;
		};

	private:
		utility::ConcurrentQueue<std::unique_ptr<BaseTask>> tasks;
		std::condition_variable hasTask;
		std::vector<Worker*> workers;

	private:
		void workerThread(Worker* worker);

		std::unique_ptr<Future> addTask(std::unique_ptr<BaseTask>&& task);

	public:
		/// @brief Construct ThreadPool
		/// @param threadCount Number of threads in ThreadPool(default is max threads for current hardware)
		ThreadPool(size_t threadsCount = std::thread::hardware_concurrency());

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
		std::unique_ptr<Future> addTask(R(*task)(ArgsT...), const std::function<void()>& callback, Args&&... args);

		/// @brief Add new task to thread pool
		template<typename R, typename... ArgsT, typename... Args>
		std::unique_ptr<Future> addTask(const std::function<R(ArgsT...)>& task, std::function<void()>&& callback, Args&&... args);

		/// @brief Add new task to thread pool
		template<typename R, typename... ArgsT, typename... Args>
		std::unique_ptr<Future> addTask(R(*task)(ArgsT...), std::function<void()>&& callback, Args&&... args);

		/**
		* @brief Create custom new task of type TaskT and add that task to thread pool
		*/
		template<typename TaskT, typename... Args>
		std::unique_ptr<Future> addTask(Args&&... args) requires std::derived_from<TaskT, BaseTask>;

		/// @brief Reinitialize thread pool
		/// @param wait Wait all threads execution
		/// @param threadsCount New thread pool size
		void reinit(bool wait = true, size_t threadsCount = std::thread::hardware_concurrency());

		/// @brief Change thread pool size
		bool resize(size_t threadsCount);

		/// @brief Stop ThreadPool
		/// @param wait Wait all threads execution
		void shutdown(bool wait = true);

		/// @brief Check is thread pool has task that running in some thread
		/// @return Returns true if thread pool has task
		bool isAnyTaskRunning() const;

		/// @brief Check specific thread state
		/// @param threadIndex Index of thread between 0 and threadsCount
		/// @return Thread state
		/// @exception std::out_of_range
		threadState getThreadState(size_t threadIndex) const;

		/// @brief Check specific thread progress
		/// @param threadIndex Index of thread between 0 and threadsCount
		/// @return Thread progress or -1 if thread not running any task
		/// @exception std::out_of_range
		float getThreadProgress(size_t threadIndex) const;

		/// @brief Getter for threadsCount
		/// @return Current count of threads in thread pool
		size_t getThreadsCount() const;

		/// @brief Getter for threadsCount
		/// @return Current count of threads in thread pool
		size_t size() const;

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
	std::unique_ptr<Future> ThreadPool::addTask(R(*task)(ArgsT...), const std::function<void()>& callback, Args&&... args)
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

	template<typename R, typename... ArgsT, typename... Args>
	std::unique_ptr<Future> ThreadPool::addTask(R(*task)(ArgsT...), std::function<void()>&& callback, Args&&... args)
	{
		return this->addTask
		(
			std::make_unique<FunctionWrapperTask<R>>(std::bind(task, std::forward<Args>(args)...), std::move(callback))
		);
	}

	template<typename TaskT, typename... Args>
	std::unique_ptr<Future> ThreadPool::addTask(Args&&... args) requires std::derived_from<TaskT, BaseTask>
	{
		return this->addTask
		(
			std::make_unique<TaskT>(std::forward<Args>(args)...)
		);
	}
}
