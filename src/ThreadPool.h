#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>
#include <future>

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
	public:
		enum class threadState
		{
			running,
			waiting
		};

	private:
		struct threadPoolTask
		{
			std::function<void()> task;
			std::function<void()> callback;
			std::promise<void> notify;

		public:
			threadPoolTask() = default;

			threadPoolTask(const threadPoolTask& other) = delete;

			threadPoolTask(threadPoolTask&& other) noexcept;

			threadPoolTask& operator = (const threadPoolTask& other) = delete;

			threadPoolTask& operator = (threadPoolTask&& other) noexcept;

			~threadPoolTask() = default;
		};

	private:
		std::queue<threadPoolTask> tasks;
		std::condition_variable hasTask;
		std::mutex tasksMutex;
		std::vector<std::jthread> threads;
		std::vector<threadState> threadStates;
		bool terminate;

	private:
		void workerThread(size_t threadIndex);

		std::future<void> addTask(threadPoolTask&& task);

	public:
		/// @brief Construct ThreadPool
		/// @param threadCount Number of threads in ThreadPool(default is max threads for current hardware)
		ThreadPool(uint32_t threadsCount = std::thread::hardware_concurrency());

		/// @brief Add new task to thread pool
		std::future<void> addTask(const std::function<void()>& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to thread pool
		std::future<void> addTask(const std::function<void()>& task, std::function<void()>&& callback = nullptr);

		/// @brief Add new task to thread pool
		std::future<void> addTask(std::function<void()>&& task, const std::function<void()>& callback = nullptr);

		/// @brief Add new task to thread pool
		std::future<void> addTask(std::function<void()>&& task, std::function<void()>&& callback = nullptr);

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
}
