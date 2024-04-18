cmake_minimum_required(VERSION 3.16)

project(MyProject)

file(GLOB SOURCES "src/*.cpp" "src/*.hpp")

find_package(websocketpp CONFIG REQUIRED)

add_executable(MyExecutable ${SOURCES})

target_link_libraries(MyExecutable PRIVATE websocketpp::websocketpp)