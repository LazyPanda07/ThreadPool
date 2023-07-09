#pragma once

#include "Promise.h"

#include "FunctionWrapperFuture.h"

namespace threading
{
	template<typename R>
	class THREAD_POOL_API FunctionWrapperPromise : public Promise
	{
	protected:
		std::promise<R> implementation;

	public:
		FunctionWrapperPromise() = default;

		virtual void notify() override;

		std::promise<R>& getPromise();

		virtual std::unique_ptr<Future> getFuture() override;

		virtual ~FunctionWrapperPromise() = default;
	};

	template<typename R>
	void FunctionWrapperPromise<R>::notify()
	{
		
	}

	template<typename R>
	std::promise<R>& FunctionWrapperPromise<R>::getPromise()
	{
		return implementation;
	}

	template<typename R>
	std::unique_ptr<Future> FunctionWrapperPromise<R>::getFuture()
	{
		return std::make_unique<FunctionWrapperFuture<R>>(implementation.get_future());
	}
}
