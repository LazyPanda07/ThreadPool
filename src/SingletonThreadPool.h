#pragma once

#include "ThreadPool.h"

namespace threading
{
	/// @brief Singleton version of ThreadPool
	template<size_t threadsCount>
	class THREAD_POOL_API SingletonThreadPool final
	{
	private:
		ThreadPool threadPool;

	private:
		SingletonThreadPool();

		~SingletonThreadPool() = default;

	public:
		/// @brief Singleton get instance method
		/// @return Instance for current template argument
		static SingletonThreadPool& get();

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
		
		/**
		 * @brief Reinitialize thread pool
		 * @param wait Wait all threads execution
		*/
		void reinit(bool wait = true);

		/**
		 * @brief Stop ThreadPool
		 * @param wait Wait all threads execution
		*/
		void shutdown(bool wait = true);

		/// @brief Check is thread pool has task that running in some thread
		/// @return Returns true if thread pool has task
		bool isAnyTaskRunning() const;

		/// @brief Getter for threadsCount
		/// @return Current count of threads in thread pool
		size_t getThreadsCount() const;

		/// @brief Getter for threadsCount
		/// @return Current count of threads in thread pool
		size_t size() const;
	};

	template<size_t threadsCount>
	SingletonThreadPool<threadsCount>::SingletonThreadPool() :
		threadPool(threadsCount)
	{

	}

	template<size_t threadsCount>
	SingletonThreadPool<threadsCount>& SingletonThreadPool<threadsCount>::get()
	{
		static SingletonThreadPool<threadsCount> instance;

		return instance;
	}

	template<size_t threadsCount>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(const std::function<void()>& task, const std::function<void()>& callback)
	{
		return threadPool.addTask(task, callback);
	}

	template<size_t threadsCount>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(const std::function<void()>& task, std::function<void()>&& callback)
	{
		return threadPool.addTask(task, std::move(callback));
	}

	template<size_t threadsCount>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(std::function<void()>&& task, const std::function<void()>& callback)
	{
		return threadPool.addTask(std::move(task), callback);
	}

	template<size_t threadsCount>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(std::function<void()>&& task, std::function<void()>&& callback)
	{
		return threadPool.addTask(std::move(task), std::move(callback));
	}

	template<size_t threadsCount>
	template<typename R, typename... ArgsT, typename... Args>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(const std::function<R(ArgsT...)>& task, const std::function<void()>& callback, Args&&... args)
	{
		return threadPool.addTask(task, callback, std::forward<Args>(args)...);
	}

	template<size_t threadsCount>
	template<typename R, typename... ArgsT, typename... Args>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(R(*task)(ArgsT...), const std::function<void()>& callback, Args&&... args)
	{
		return threadPool.addTask(task, callback, std::forward<Args>(args)...);
	}

	template<size_t threadsCount>
	template<typename R, typename... ArgsT, typename... Args>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(const std::function<R(ArgsT...)>& task, std::function<void()>&& callback, Args&&... args)
	{
		return threadPool.addTask(task, std::move(callback), std::forward<Args>(args)...);
	}

	template<size_t threadsCount>
	template<typename R, typename... ArgsT, typename... Args>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(R(*task)(ArgsT...), std::function<void()>&& callback, Args&&... args)
	{
		return threadPool.addTask(task, std::move(callback), std::forward<Args>(args)...);
	}

	template<size_t threadsCount>
	template<typename TaskT, typename... Args>
	std::unique_ptr<Future> SingletonThreadPool<threadsCount>::addTask(Args&&... args) requires std::derived_from<TaskT, BaseTask>
	{
		return threadPool.addTask<TaskT>(std::forward<Args>(args)...);
	}

	template<size_t threadsCount>
	void SingletonThreadPool<threadsCount>::reinit(bool wait)
	{
		threadPool.reinit(wait, threadsCount);
	}

	template<size_t threadsCount>
	void SingletonThreadPool<threadsCount>::shutdown(bool wait)
	{
		threadPool.shutdown(wait);
	}

	template<size_t threadsCount>
	bool SingletonThreadPool<threadsCount>::isAnyTaskRunning() const
	{
		return threadPool.isAnyTaskRunning();
	}

	template<size_t threadsCount>
	size_t SingletonThreadPool<threadsCount>::getThreadsCount() const
	{
		return threadsCount;
	}

	template<size_t threadsCount>
	size_t SingletonThreadPool<threadsCount>::size() const
	{
		return threadsCount;
	}
}
