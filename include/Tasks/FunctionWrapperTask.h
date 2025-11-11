#pragma once

#include "BaseTask.h"

#include <functional>

#include "Utility/FunctionWrapperPromise.h"

namespace threading
{
	template<typename R, typename... ArgsT>
	class THREAD_POOL_API FunctionWrapperTask : public BaseTask
	{
	protected:
		std::function<R(ArgsT...)> executeFunction;
		std::function<void()> callbackFunction;

	protected:
		virtual void executeImplementation() override;

		virtual std::unique_ptr<Promise> createTaskPromise() const override;

	public:
		FunctionWrapperTask(const std::function<R(ArgsT...)>& executeFunction, const std::function<void()>& callbackFunction);

		FunctionWrapperTask(const std::function<R(ArgsT...)>& executeFunction, std::function<void()>&& callbackFunction);

		FunctionWrapperTask(std::function<R(ArgsT...)>&& executeFunction, const std::function<void()>& callbackFunction);

		FunctionWrapperTask(std::function<R(ArgsT...)>&& executeFunction, std::function<void()>&& callbackFunction);

		virtual void execute() override;

		virtual ~FunctionWrapperTask() = default;
	};

	template<typename R, typename... ArgsT>
	void FunctionWrapperTask<R, ArgsT...>::executeImplementation()
	{
		
	}

	template<typename R, typename... ArgsT>
	std::unique_ptr<Promise> FunctionWrapperTask<R, ArgsT...>::createTaskPromise() const
	{
		return std::make_unique<FunctionWrapperPromise<R>>();
	}

	template<typename R, typename... ArgsT>
	FunctionWrapperTask<R, ArgsT...>::FunctionWrapperTask(const std::function<R(ArgsT...)>& executeFunction, const std::function<void()>& callbackFunction) :
		executeFunction(executeFunction),
		callbackFunction(callbackFunction)
	{

	}

	template<typename R, typename... ArgsT>
	FunctionWrapperTask<R, ArgsT...>::FunctionWrapperTask(const std::function<R(ArgsT...)>& executeFunction, std::function<void()>&& callbackFunction) :
		executeFunction(executeFunction),
		callbackFunction(std::move(callbackFunction))
	{

	}

	template<typename R, typename... ArgsT>
	FunctionWrapperTask<R, ArgsT...>::FunctionWrapperTask(std::function<R(ArgsT...)>&& executeFunction, const std::function<void()>& callbackFunction) :
		executeFunction(std::move(executeFunction)),
		callbackFunction(callbackFunction)
	{

	}

	template<typename R, typename... ArgsT>
	FunctionWrapperTask<R, ArgsT...>::FunctionWrapperTask(std::function<R(ArgsT...)>&& executeFunction, std::function<void()>&& callbackFunction) :
		executeFunction(std::move(executeFunction)),
		callbackFunction(std::move(callbackFunction))
	{

	}

	template<typename R, typename... ArgsT>
	void FunctionWrapperTask<R, ArgsT...>::execute()
	{
		if constexpr (std::is_same_v<R, void>)
		{
			executeFunction();

			if (callbackFunction)
			{
				callbackFunction();
			}

			dynamic_cast<FunctionWrapperPromise<R>&>(*taskPromise).getPromise().set_value();
		}
		else
		{
			R result = executeFunction();

			if (callbackFunction)
			{
				callbackFunction();
			}

			dynamic_cast<FunctionWrapperPromise<R>&>(*taskPromise).getPromise().set_value(result);
		}
	}
}
