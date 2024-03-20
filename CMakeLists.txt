cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dir CACHE STRING "")
option(BUILD_SHARED_LIBS "" OFF)

if (UNIX)
    add_definitions(-D__LINUX__)
endif (UNIX)

if (${BUILD_SHARED_LIBS})
    add_definitions(-DTHREAD_POOL_DLL)
endif ()

project(ThreadPool)

add_library(
    ${PROJECT_NAME}
    src/ThreadPool.cpp
    src/SingletonThreadPool.cpp
    src/Utility/Promise.cpp
    src/Utility/Future.cpp
    src/Tasks/BaseTask.cpp
)

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
endif ()
