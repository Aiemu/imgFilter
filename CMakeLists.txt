cmake_minimum_required(VERSION 3.20)
project(filter)

set(CMAKE_CXX_STANDARD 14)

add_executable(filter main.cpp include/SLIC.cpp include/SLIC.h include/LOMO.cpp include/LOMO.h)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#find_library(OpenCV)
find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(filter ${OpenCV_LIBS})