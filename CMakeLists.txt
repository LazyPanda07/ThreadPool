cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
option(BUILD_SHARED_LIBS "" OFF)

if (UNIX)
    add_definitions(-D__LINUX__)
endif (UNIX)

if (${BUILD_SHARED_LIBS})
    add_definitions(-DTHREAD_POOL_DLL)
endif ()

project(ThreadPool VERSION 1.8.0)

add_library(
    ${PROJECT_NAME}
    src/ThreadPool.cpp
    src/Utility/Promise.cpp
    src/Tasks/BaseTask.cpp
)

if (DEFINED ENV{MARCH} AND NOT "$ENV{MARCH}" STREQUAL "")
    target_compile_options(${PROJECT_NAME} PRIVATE -march=$ENV{MARCH})
endif()

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    include
)

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION dll
)

install(DIRECTORY include DESTINATION .)
