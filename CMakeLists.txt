cmake_minimum_required(VERSION 3.10)
project(ImageTransform)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable for OpenCV test
add_executable(main src/main.cpp)
target_link_libraries(main ${OpenCV_LIBS})

