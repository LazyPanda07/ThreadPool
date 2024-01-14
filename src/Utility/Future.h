#pragma once

#include <any>

#ifdef THREAD_POOL_DLL
#ifdef __LINUX__
__attribute__((visibility("default")))
#else
#define THREAD_POOL_API __declspec(dllexport)
#endif
#pragma warning(disable: 4251)
#else
#define THREAD_POOL_API
#endif // THREAD_POOL_DLL

namespace threading
{
	class THREAD_POOL_API Future
	{
	protected:
		virtual std::any getValue() const = 0;

	public:
		Future() = default;

		virtual void wait() = 0;

		template<typename T>
		T get() const;

		virtual ~Future() = default;
	};

	template<typename T>
	T Future::get() const
	{
		return std::any_cast<T>(this->getValue());
	}
}
