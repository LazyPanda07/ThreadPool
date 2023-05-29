#pragma once

#include "ThreadPool.h"

namespace threading
{
	/// @brief Singleton version of ThreadPool
	template<uint32_t threadsCount>
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
		std::future<void> addTask(const std::function<void()>& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to thread pool
		std::future<void> addTask(const std::function<void()>& task, std::function<void()>&& callback = nullptr);

		/// @brief Add new task to thread pool
		std::future<void> addTask(std::function<void()>&& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to thread pool
		std::future<void> addTask(std::function<void()>&& task, std::function<void()>&& callback = nullptr);

		/// @brief Reinitialize thread pool
		void reinit();

		/// @brief Stop ThreadPool with joining all threads
		void shutdown();

		/// @brief Check is thread pool has task that running in some thread
		/// @return Returns true if thread pool has task
		bool isAnyTaskRunning() const;

		/// @brief Getter for threadsCount
		/// @return Current count of threads in thread pool
		uint32_t getThreadsCount() const;
	};

	template<uint32_t threadsCount>
	SingletonThreadPool<threadsCount>::SingletonThreadPool() :
		threadPool(threadsCount)
	{

	}

	template<uint32_t threadsCount>
	SingletonThreadPool<threadsCount>& SingletonThreadPool<threadsCount>::get()
	{
		static SingletonThreadPool<threadsCount> instance;

		return instance;
	}

	template<uint32_t threadsCount>
	std::future<void> SingletonThreadPool<threadsCount>::addTask(const std::function<void()>& task, const std::function<void()>& callback)
	{
		return threadPool.addTask(task, callback);
	}

	template<uint32_t threadsCount>
	std::future<void> SingletonThreadPool<threadsCount>::addTask(const std::function<void()>& task, std::function<void()>&& callback)
	{
		return threadPool.addTask(task, std::move(callback));
	}

	template<uint32_t threadsCount>
	std::future<void> SingletonThreadPool<threadsCount>::addTask(std::function<void()>&& task, const std::function<void()>& callback)
	{
		return threadPool.addTask(std::move(task), callback);
	}

	template<uint32_t threadsCount>
	std::future<void> SingletonThreadPool<threadsCount>::addTask(std::function<void()>&& task, std::function<void()>&& callback)
	{
		return threadPool.addTask(std::move(task), std::move(callback));
	}

	template<uint32_t threadsCount>
	void SingletonThreadPool<threadsCount>::reinit()
	{
		threadPool.reinit();
	}

	template<uint32_t threadsCount>
	void SingletonThreadPool<threadsCount>::shutdown()
	{
		threadPool.shutdown();
	}

	template<uint32_t threadsCount>
	bool SingletonThreadPool<threadsCount>::isAnyTaskRunning() const
	{
		threadPool.isAnyTaskRunning();
	}

	template<uint32_t threadsCount>
	uint32_t SingletonThreadPool<threadsCount>::getThreadsCount() const
	{
		threadsCount;
	}
}
