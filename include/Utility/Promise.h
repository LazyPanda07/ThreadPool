#pragma once

#include <memory>

#include "Future.h"

namespace threading
{
	class THREAD_POOL_API Promise
	{
	public:
		Promise() = default;

		virtual void notify() = 0;

		virtual std::unique_ptr<Future> getFuture() = 0;

		virtual ~Promise() = default;
	};
}
