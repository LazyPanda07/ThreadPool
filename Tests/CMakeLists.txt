cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin)
set(GTEST_VERSION 1.17.0)
option(VALGRIND "Build with valgrind" OFF)

project(Tests)

include(FetchContent)

FetchContent_Declare(
	gtest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v${GTEST_VERSION}
)

FetchContent_MakeAvailable(gtest)

if (UNIX)
	set(SHARED_OBJECT ${PROJECT_SOURCE_DIR}/../ThreadPool/lib/libThreadPool.so)
else()
	set(SHARED_OBJECT ${PROJECT_SOURCE_DIR}/../ThreadPool/dll/ThreadPool.dll)
endif()

add_definitions(-D__LINUX__)

if (${VALGRIND})
	add_definitions(-D__VALGRIND__)
endif()

add_executable(
    ${PROJECT_NAME} 
    src/Main.cpp 
    src/Functions.cpp 
    src/ThreadPoolTest.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/../include
	include
)

target_link_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/../ThreadPool/lib
)
target_link_libraries(
	${PROJECT_NAME}
	ThreadPool
	gtest
	gtest_main
)

install(TARGETS ${PROJECT_NAME} DESTINATION .)

if(EXISTS ${SHARED_OBJECT})
    install(FILES ${SHARED_OBJECT} DESTINATION .)
endif()
