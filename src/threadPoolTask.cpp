#include "ThreadPool.h"

using namespace std;

namespace threading
{
	ThreadPool::threadPoolTask::threadPoolTask(const threadPoolTask& other)
	{
		(*this) = other;
	}

	ThreadPool::threadPoolTask::threadPoolTask(threadPoolTask&& other) noexcept
	{
		(*this) = move(other);
	}

	ThreadPool::threadPoolTask& ThreadPool::threadPoolTask::operator = (const threadPoolTask& other)
	{
		task = other.task;
		callback = other.callback;

		return *this;
	}

	ThreadPool::threadPoolTask& ThreadPool::threadPoolTask::operator = (threadPoolTask&& other) noexcept
	{
		task = move(other.task);
		callback = move(other.callback);

		return *this;
	}
}
