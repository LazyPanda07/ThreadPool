cmake_minimum_required(VERSION 3.27.0)

project(Tests)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/../src/
	${PROJECT_SOURCE_DIR}/gtest/include/
	${PROJECT_SOURCE_DIR}/include/
)

target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/gtest/lib ${PROJECT_SOURCE_DIR}/../ThreadPool)
target_link_libraries(${PROJECT_NAME} gtest gtest_main ThreadPool)

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

	install(TARGETS ${PROJECT_NAME} DESTINATION .)
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

	install(TARGETS ${PROJECT_NAME} DESTINATION .)
endif()

add_executable(
    ${PROJECT_NAME} 
    src/Main.cpp 
    src/Functions.cpp 
    src/SingletonThreadPoolTest.cpp 
    src/ThreadPoolTest.cpp
)