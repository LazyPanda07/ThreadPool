#pragma once

#include "Utility/Promise.h"

namespace threading
{
	/**
	 * @brief Base class for all task in ThreadPool
	*/
	class THREAD_POOL_API BaseTask
	{
	protected:
		std::unique_ptr<Promise> taskPromise;

	protected:
		virtual void executeImplementation() = 0;

		virtual std::unique_ptr<Promise> createTaskPromise() const = 0;

		virtual void callback();

		virtual void notifyFuture();

	public:
		BaseTask() = default;

		virtual void execute();

		virtual std::unique_ptr<Future> getFuture();

		virtual float getProgress() const;

		virtual ~BaseTask() = default;

		friend class ThreadPool;
	};
}
