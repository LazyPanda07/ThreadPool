#include "Functions.h"

#include <thread>

int64_t sum(int64_t left, int64_t right)
{
	int64_t result = 0;

	for (int64_t i = left; i < right; i++)
	{
		result += i;
	}

	return result;
}

void longCalculation(int& result)
{
	using namespace std::chrono_literals;

	std::this_thread::sleep_for(10s);

	result = 10;
}

size_t get(size_t value)
{
	return value;
}
