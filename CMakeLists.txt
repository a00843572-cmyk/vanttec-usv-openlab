cmake_minimum_required(VERSION 3.16)

project(vanttec_usv_openlab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(usv_vision
    src/main.cpp
)