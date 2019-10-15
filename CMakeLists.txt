cmake_minimum_required(VERSION 3.14)
project(NRT)

# Check IPO Support
cmake_policy(SET CMP0069 NEW)
include(CheckIPOSupported)
check_ipo_supported(RESULT NWCONF_IPO_SUPPORT OUTPUT NWCONF_IPO_SUPPORT_MESSAGE)
if (NWCONF_IPO_SUPPORT)
    message(STATUS "IPO IS SUPPORTED, ENABLED")
else()
    message(STATUS "IPO IS NOT SUPPORTED: ${NWCONF_IPO_SUPPORT_MESSAGE}, DISABLED")
endif ()

function(target_enable_ipo NAME)
    if (NWCONF_IPO_SUPPORT)
        set_property(TARGET ${NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION $<$<CONFIG:Debug>:FALSE>:TRUE)
    endif ()
endfunction()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_subdirectory(Core)
add_subdirectory(Conc)

file(GLOB_RECURSE SOURCE Source/*.*)
add_executable(NRT ${SOURCE})
target_enable_ipo(NRT)
target_include_directories(NRT PRIVATE Source)
target_link_libraries(NRT PRIVATE NRT.Core NRT.Conc)
