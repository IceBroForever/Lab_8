cmake_minimum_required(VERSION 2.8.7)

set (CMAKE_CXX_STANDARD 11)

include_directories(include include)
file(GLOB LIB_SOURCES "src/*.cpp")

project (server)
add_executable(${CMAKE_PROJECT_NAME} "server.cpp" ${LIB_SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} jansson progbase-cpp progbase ssl)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Werror -Wall")