cmake_minimum_required(VERSION 3.14)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.*)
add_library(NRT.GL.OpenGL SHARED ${SOURCE})
target_enable_ipo(NRT.GL.OpenGL)
target_compile_definitions(NRT.GL.OpenGL PRIVATE -DNRTOGL_BUILD)
target_include_directories(NRT.GL.OpenGL PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(NRT.GL.OpenGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_include_directories(NRT.GL.OpenGL INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(NRT.GL.OpenGL PUBLIC NRT.Core)
