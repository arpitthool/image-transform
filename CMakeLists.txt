cmake_minimum_required(VERSION 3.10)
project(ImageTransform)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable for OpenCV test
add_executable(image_transform image_transform.cpp)
target_link_libraries(image_transform ${OpenCV_LIBS})

