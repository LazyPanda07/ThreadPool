#include "gtest/gtest.h"

#pragma comment(lib, "gtest.lib")
#pragma comment(lib, "gtest_main.lib")
#pragma comment(lib, "ThreadPool.lib")

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
