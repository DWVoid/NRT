cmake_minimum_required(VERSION 3.11)
project(NRT)

# Check IPO Support
if (NOT DEFINED NWCONF_IPO_CONFIGURED)
    set(NWCONF_IPO_CONFIGURED TRUE)
    cmake_policy(SET CMP0069 NEW)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT NWCONF_IPO_SUPPORT OUTPUT NWCONF_IPO_SUPPORT_MESSAGE)
    if (NWCONF_IPO_SUPPORT)
        message(STATUS "IPO IS SUPPORTED, ENABLED")
    else()
        message(STATUS "IPO IS NOT SUPPORTED: ${NWCONF_IPO_SUPPORT_MESSAGE}, DISABLED")
    endif()

    function(target_enable_ipo NAME)
        if (NWCONF_IPO_SUPPORT)
            set_property(TARGET ${NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION $<$<CONFIG:Debug>:FALSE>:TRUE)
        endif ()
    endfunction()
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_subdirectory(Core)

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
    target_link_libraries(NRT PRIVATE NRT.Core NRT.Conc)
endif()
