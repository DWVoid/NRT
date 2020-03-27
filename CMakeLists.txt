cmake_minimum_required(VERSION 3.11)
project(NRT)

set(CMAKE_CXX_STANDARD 17)

include(Build/Configure.cmake)

nw_add_subdirectory(Crt)
nw_add_subdirectory(Cfx)
nw_add_subdirectory(Math)

if (NRT_BUILD_ALL OR NRT_BUILD_CONC)
    nw_add_subdirectory(Conc)
endif()

if (NRT_BUILD_ALL OR NRT_BUILD_MSG)
    nw_add_subdirectory(Msg)
endif()

if (NRT_BUILD_ALL OR NRT_BUILD_OGL)
    nw_add_subdirectory(GL.OpenGL)
endif()

if (NRT_BUILD_ALL)
    nw_add_subdirectory(Tests)
endif()

if (NRT_BUILD_ALL)
    file(GLOB_RECURSE SOURCE Source/*.*)
    add_executable(NRT ${SOURCE})
    target_enable_ipo(NRT)
    target_include_directories(NRT PRIVATE Source)
    target_link_libraries(NRT PRIVATE NRT.CFX NRT.Conc)
endif()
