cmake_minimum_required(VERSION 3.12)
project(Car-Simulation)

set(RAYLIB_PATH "include/raylib")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}
    src/main.cpp
)  

find_library(RAYLIB_LIBRARY raylib HINTS ${RAYLIB_PATH})

if (NOT RAYLIB_LIBRARY)
    message(FATAL_ERROR "raylib library not found in ${RAYLIB_PATH}")
endif()

target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARY})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
