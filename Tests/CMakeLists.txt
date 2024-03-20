cmake_minimum_required(VERSION 3.27.0)

project(Tests)

set(CMAKE_CXX_STANDARD 20)
set(SHARED_OBJECT ${PROJECT_SOURCE_DIR}/../ThreadPool/libThreadPool.so)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin)
option(VALGRIND "Build with valgrind" OFF)

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
	${PROJECT_SOURCE_DIR}/gtest/include/
	${PROJECT_SOURCE_DIR}/include/
)

target_link_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/gtest/lib 
	${PROJECT_SOURCE_DIR}/../ThreadPool/lib
)
target_link_libraries(
	${PROJECT_NAME} 
	gtest 
	gtest_main 
	ThreadPool
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

if(EXISTS ${SHARED_OBJECT})
    install(FILES ${SHARED_OBJECT} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
