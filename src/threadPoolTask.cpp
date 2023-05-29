#include "ThreadPool.h"

using namespace std;

namespace threading
{
	ThreadPool::threadPoolTask::threadPoolTask(threadPoolTask&& other) noexcept
	{
		(*this) = move(other);
	}

	ThreadPool::threadPoolTask& ThreadPool::threadPoolTask::operator = (threadPoolTask&& other) noexcept
	{
		task = move(other.task);
		callback = move(other.callback);
		notify = move(other.notify);

		return *this;
	}
}
