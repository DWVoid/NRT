cmake_minimum_required(VERSION 3.11)
project(NRT)

set(CMAKE_CXX_STANDARD 17)

include(Build/Configure.cmake)

add_subdirectory(Crt)
add_subdirectory(Cfx)

if (NRT_BUILD_ALL OR NRT_BUILD_CONC)
    add_subdirectory(Conc)
endif()

if (NRT_BUILD_ALL OR NRT_BUILD_MSG)
    add_subdirectory(Msg)
endif()

if (NRT_BUILD_ALL OR NRT_BUILD_OGL)
    add_subdirectory(GL.OpenGL)
endif()

# Add GTest
if (NRT_BUILD_ALL OR NRT_BUILD_TEST)
    include(FetchContent)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.8.1
    )
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(Tests)
endif()

if (NRT_BUILD_ALL)
    file(GLOB_RECURSE SOURCE Source/*.*)
    add_executable(NRT ${SOURCE})
    target_enable_ipo(NRT)
    target_include_directories(NRT PRIVATE Source)
    target_link_libraries(NRT PRIVATE NRT.CFX NRT.Conc)
endif()
