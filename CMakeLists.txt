cmake_minimum_required(VERSION 3.16)

project(vanttec_usv_vision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(
    usv_vision
    src/main.cpp
    src/ObjectDetector.cpp
    src/DepthEstimator.cpp
)

target_link_libraries(
    usv_vision
    ${OpenCV_LIBS}
)