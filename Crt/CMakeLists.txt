cmake_minimum_required(VERSION 3.11)

find_package(Threads REQUIRED)
file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.*)
add_library(NRT.CRT SHARED ${SOURCE})
target_enable_ipo(NRT.CRT)
target_compile_definitions(NRT.CRT PRIVATE -DNRTCRT_BUILD)
target_include_directories(NRT.CRT PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_include_directories(NRT.CRT INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(NRT.CRT PRIVATE Threads::Threads)
