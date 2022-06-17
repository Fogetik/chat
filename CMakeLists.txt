cmake_minimum_required(VERSION 3.22)
project(chat)

set(CMAKE_CXX_STANDARD 23)
link_libraries(ws2_32 wsock32)

add_executable(chat main.cpp)
