cmake_minimum_required(VERSION 3.27)

project(c8c)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED YES)

file(GLOB_RECURSE SOURCES src/**.c)
file(GLOB_RECURSE HEADERS include/**.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)

if(BUILD_FOR_NATIVE_CPU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif()

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::Main)

find_package(SDL2_mixer REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::Mixer)
