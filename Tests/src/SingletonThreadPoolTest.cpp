#include "gtest/gtest.h"

#include "Functions.h"

#include "SingletonThreadPool.h"

#define ADD_TASK(number) \
TEST(SingletonThreadPool, AddTask##number##Threads) \
{ \
	auto& threadPool = threading::SingletonThreadPool<number>::get(); \
	std::vector<std::unique_ptr<threading::Future>> result; \
	\
	result.emplace_back(threadPool.addTask(sum, nullptr, 0, 10)); \
	result.emplace_back(threadPool.addTask(sum, nullptr, 10, 20)); \
	result.emplace_back(threadPool.addTask(sum, nullptr, 20, 30)); \
	result.emplace_back(threadPool.addTask(sum, nullptr, 30, 40)); \
	\
	for (const std::unique_ptr<threading::Future>& future : result) \
	{ \
		int64_t value = future->get<int64_t>(); \
		\
		ASSERT_TRUE(value == sum(0, 10) || value == sum(10, 20) || value == sum(20, 30) || value == sum(30, 40)); \
	} \
	\
	threadPool.shutdown(); \
}

ADD_TASK(2)

ADD_TASK(3)

ADD_TASK(4)

ADD_TASK(5)

ADD_TASK(6)

ADD_TASK(7)

ADD_TASK(8)
