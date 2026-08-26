cmake_minimum_required(VERSION 3.16)

project(smartproxy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/http)

add_executable(smartproxy
    src/main.cpp
    src/http/parser.cpp
    src/lb/balancer.cpp
)