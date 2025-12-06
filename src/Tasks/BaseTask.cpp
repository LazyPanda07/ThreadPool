#include "Tasks/BaseTask.h"

#include <future>

namespace threading
{
	void BaseTask::callback()
	{

	}

	void BaseTask::notifyFuture()
	{
		taskPromise->notify();
	}

	void BaseTask::execute()
	{
		this->executeImplementation();

		this->callback();

		this->notifyFuture();
	}

	std::unique_ptr<Future> BaseTask::getFuture()
	{
		return taskPromise->getFuture();
	}

	float BaseTask::getProgress() const
	{
		return 0.0f;
	}
}
