#include "gtest/gtest.h"

#include <random>
#include <chrono>

#include "Functions.h"

#include "ThreadPool.h"

#define CALCULATE_TIME(function, result) \
{ \
	auto start = std::chrono::high_resolution_clock::now(); \
	function; \
	result = static_cast<double>((std::chrono::high_resolution_clock::now() - start).count()) / std::chrono::high_resolution_clock::period::den; \
}

TEST(ThreadPool, AddTask)
{
	threading::ThreadPool threadPool(4);
	std::vector<std::unique_ptr<threading::Future>> result;

	result.emplace_back(threadPool.addTask(sum, nullptr, 0, 10));
	result.emplace_back(threadPool.addTask(sum, nullptr, 10, 20));
	result.emplace_back(threadPool.addTask(sum, nullptr, 20, 30));
	result.emplace_back(threadPool.addTask(sum, nullptr, 30, 40));

	for (const std::unique_ptr<threading::Future>& future : result)
	{
		int64_t value = future->get<int64_t>();

		ASSERT_TRUE(value == sum(0, 10) || value == sum(10, 20) || value == sum(20, 30) || value == sum(30, 40));
	}
}

TEST(ThreadPool, AddTaskWithCallback)
{
	using namespace std::chrono_literals;

	threading::ThreadPool threadPool(4);
	std::vector<std::unique_ptr<threading::Future>> result;
	std::atomic<int> finish;

	for (int64_t i = 0; i < static_cast<int64_t>(threadPool.size()); i++)
	{
		result.emplace_back(threadPool.addTask(sum, [&finish]() { std::this_thread::sleep_for(1s); finish++; }, i * 10, (i + 1) * 10));
	}

	while (finish != 4);

	for (const std::unique_ptr<threading::Future>& future : result)
	{
		int64_t value = future->get<int64_t>();

		ASSERT_TRUE(value == sum(0, 10) || value == sum(10, 20) || value == sum(20, 30) || value == sum(30, 40));
	}
}

TEST(ThreadPool, Speed)
{
#if defined(_DEBUG) || defined(__VALGRIND__)
	return;
#endif

	threading::ThreadPool threadPool(8);
	std::mt19937_64 random(std::time(nullptr));
	int64_t left = random() % 100'000'000;
	int64_t right = left + 50'000'000;
	int64_t firstValue = 0;
	int64_t secondValue = 0;
	double first = 0.0;
	double second = 0.0;

	auto threadPoolCalculation = [&threadPool](int64_t left, int64_t right) -> int64_t
		{
			int64_t min = std::min(left, right);
			int64_t max = std::max(left, right);
			int64_t perThread = static_cast<int64_t>(std::ceil(static_cast<double>(max - min) / static_cast<double>(threadPool.size())));
			int64_t result = 0;
			std::vector<std::unique_ptr<threading::Future>> futures;

			for (size_t i = 0; i < threadPool.size() - 1; i++)
			{
				futures.emplace_back(threadPool.addTask(sum, nullptr, min + i * perThread, min + (i + 1) * perThread));
			}

			futures.emplace_back(threadPool.addTask(sum, nullptr, min + static_cast<int64_t>((threadPool.size() - 1)) * perThread, max));

			for (const std::unique_ptr<threading::Future>& value : futures)
			{
				result += value->get<int64_t>();
			}

			return result;
		};

	CALCULATE_TIME(firstValue = threadPoolCalculation(std::min(left, right), std::max(left, right)), first);
	CALCULATE_TIME(secondValue = sum(std::min(left, right), std::max(left, right)), second);

	std::cout << first << 's' << std::endl << second << 's' << std::endl;

	ASSERT_TRUE(firstValue == secondValue);
	ASSERT_TRUE(first < second);
}

TEST(ThreadPool, TasksSpam)
{
	threading::ThreadPool threadPool(8);

	for (int64_t i = 0; i < 1'000'000; i++)
	{
		threadPool.addTask(sum, nullptr, i, i + 10);
	}
}

TEST(ThreadPool, LongCalculation)
{
	threading::ThreadPool threadPool(4);
	int result;

	threadPool.addTask(longCalculation, nullptr, std::ref(result))->wait();

	ASSERT_TRUE(result == 10);
}
