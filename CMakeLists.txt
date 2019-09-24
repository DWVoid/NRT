cmake_minimum_required(VERSION 3.14)
project(NRT)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS context)

file(GLOB_RECURSE SOURCE Source/*.*)
add_executable(NRT ${SOURCE})
target_include_directories(NRT PRIVATE Source)
target_link_libraries(NRT PRIVATE Boost::context)
