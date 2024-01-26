cmake_minimum_required(VERSION 3.27.0)

project(Tests)

set(SHARED_OBJECT ${PROJECT_SOURCE_DIR}/../ThreadPool/libThreadPool.so)
set(VALGRIND false CACHE BOOL "Build with valgrind")

if (VALGRIND)
	add_definitions(-D__VALGRIND__)
endif()

if (${VS_BUILD_CONFIGURATION} STREQUAL Debug)
	set(VS_CONFIGURATION Debug)
	set(VS_PLATFORM x64)
    set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin/${VS_CONFIGURATION}-${VS_PLATFORM}/${PROJECT_NAME}/)

	add_definitions(
		-D_DEBUG
		-D_CONSOLE
        -D__LINUX__
	)
endif()

if (${VS_BUILD_CONFIGURATION} STREQUAL Release)
	set(VS_CONFIGURATION Release)
	set(VS_PLATFORM x64)
    set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin/${VS_CONFIGURATION}-${VS_PLATFORM}/${PROJECT_NAME}/)

	add_definitions(
		-DNDEBUG
		-D_CONSOLE
        -D__LINUX__
	)
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

target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/gtest/lib ${PROJECT_SOURCE_DIR}/../ThreadPool)
target_link_libraries(${PROJECT_NAME} gtest gtest_main ThreadPool)

install(TARGETS ${PROJECT_NAME} DESTINATION .)

if(EXISTS ${SHARED_OBJECT})
    install(FILES ${SHARED_OBJECT} DESTINATION .)
endif()
