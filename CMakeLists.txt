cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dir CACHE STRING "")
set(CMAKE_BUILD_TYPE Release CACHE STRING "")
option(BUILD_SHARED_LIBS "" OFF)

if (NOT TARGET ThreadPool)
    if (UNIX)
        add_definitions(-D__LINUX__)
    endif (UNIX)

    if (${BUILD_SHARED_LIBS})
        add_definitions(-DTHREAD_POOL_DLL)
    endif ()

    project(ThreadPool VERSION 1.7.0)

    add_library(
        ${PROJECT_NAME}
        src/ThreadPool.cpp
        src/Utility/Promise.cpp
        src/Utility/Future.cpp
        src/Tasks/BaseTask.cpp
    )

    if (DEFINED ENV{MARCH} AND NOT "$ENV{MARCH}" STREQUAL "")
    	target_compile_options(${PROJECT_NAME} PRIVATE -march=$ENV{MARCH})
    endif()

    target_include_directories(
        ${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/src/
    )

    install(
        TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
    )

    install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
endif()
