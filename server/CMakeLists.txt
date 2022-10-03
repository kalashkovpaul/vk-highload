cmake_minimum_required(VERSION 3.16)
project(server)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)
add_executable(server main.cpp include/Server.hpp include/constants.hpp src/Server.cpp src/utils.cpp include/utils.hpp include/responses.hpp)

target_link_libraries(server -lpthread)
