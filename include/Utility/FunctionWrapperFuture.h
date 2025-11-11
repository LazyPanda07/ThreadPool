#pragma once

#include "Future.h"

#include <future>

namespace threading
{
	template<typename R>
	class FunctionWrapperFuture : public Future
	{
	protected:
		mutable std::future<R> implementation;

	protected:
		virtual std::any getValue() const override;

	public:
		FunctionWrapperFuture(std::future<R>&& implementation);

		virtual void wait() override;

		virtual ~FunctionWrapperFuture() = default;
	};

	template<typename R>
	std::any FunctionWrapperFuture<R>::getValue() const
	{
		if constexpr (std::is_same_v<R, void>)
		{
			return std::any();
		}
		else
		{
			return implementation.get();
		}
	}

	template<typename R>
	FunctionWrapperFuture<R>::FunctionWrapperFuture(std::future<R>&& implementation) :
		implementation(std::move(implementation))
	{

	}

	template<typename R>
	void FunctionWrapperFuture<R>::wait()
	{
		implementation.wait();
	}
}
