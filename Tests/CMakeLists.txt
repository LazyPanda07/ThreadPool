cmake_minimum_required(VERSION 3.27.0)

project(Tests)

find_package(GTest)

if(NOT ${GTest_FOUND})
	set(GTEST_BOTH_LIBRARIES gtest gtest_main)
	set(GTEST_INCLUDE_DIRS gtest/include/)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin)
option(VALGRIND "Build with valgrind" OFF)

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
    src/SingletonThreadPoolTest.cpp 
    src/ThreadPoolTest.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/../src/
	${PROJECT_SOURCE_DIR}/include/
	${GTEST_INCLUDE_DIRS}
)

target_link_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/gtest/lib 
	${PROJECT_SOURCE_DIR}/../ThreadPool/lib
)
target_link_libraries(
	${PROJECT_NAME}
	ThreadPool
	${GTEST_BOTH_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

if(EXISTS ${SHARED_OBJECT})
    install(FILES ${SHARED_OBJECT} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
