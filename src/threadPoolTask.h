#pragma once

#include <functional>

#ifdef THREAD_POOL_DLL
#define THREAD_POOL_API __declspec(dllexport)
#else
#define THREAD_POOL_API
#endif // THREAD_POOL_DLL

namespace threading
{
	struct THREAD_POOL_API threadPoolTask
	{
		std::function<void()> task;
		std::function<void()> callback;

		threadPoolTask() = default;

		threadPoolTask(const threadPoolTask&) = default;

		threadPoolTask(threadPoolTask&&) noexcept = default;

		threadPoolTask& operator = (const threadPoolTask&) = default;

		threadPoolTask& operator = (threadPoolTask&&) noexcept = default;

		~threadPoolTask() = default;
	};
}
