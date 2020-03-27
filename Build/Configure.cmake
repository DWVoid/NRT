if (NOT DEFINED NW_CONFIG)
    set(NW_CONFIG TRUE)

    # Set Global C++ Settings to Standard 2017
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_CXX_STANDARD 17)

    # Link missing sys libs for GNU CC
    if (UNIX AND NOT APPLE)
        link_libraries(dl stdc++fs)
    endif ()

    # As NEWorld series is designed to be multi-threaded, link thread support for all
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)

    # Require vcpkg for package management
    if (VCPKG_TARGET_TRIPLET)
        message("Vcpkg detected. Active triplet is ${VCPKG_TARGET_TRIPLET}")
    else ()
        message("Vcpkg not detected.")
        if (NEWORLD_VCPKG_FORCE_OVERRIDE)
            message("Build is forced to continue without vcpkg, which is not officially supported. Good luck!")
        else ()
            message("Please add vcpkg toolchain to CMAKE_TOOLCHAIN_FILE to continue")
            return(-1)
        endif ()
    endif ()

    # Check IPO Support
    cmake_policy(SET CMP0069 NEW)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT NW_CONF_IPO_SUPPORT OUTPUT NW_CONF_IPO_SUPPORT_MESSAGE)
    if (NW_CONF_IPO_SUPPORT)
        message(STATUS "IPO IS SUPPORTED, ENABLED")
    else ()
        message(STATUS "IPO IS NOT SUPPORTED: ${NW_CONF_IPO_SUPPORT_MESSAGE}, DISABLED")
    endif ()

    function(target_enable_ipo NAME)
        if (NW_CONF_IPO_SUPPORT)
            set_property(TARGET ${NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION $<$<CONFIG:Debug>:FALSE>:TRUE)
        endif ()
    endfunction()

    function(nw_add_subdirectory NAME)
        string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} TEST)
        if (TEST)
            if (ABSOLUTE)
                file(RELATIVE_PATH NAME2 ${CMAKE_CURRENT_SOURCE_DIR} ${NAME})
            else()
                set(NAME2 ${NAME})
            endif()
            add_subdirectory(${NAME2} __1/B/${NAME2})
        else()
            add_subdirectory(${NAME})
        endif()
    endfunction()

    function(nw_add_all_subdirectory)
        file(GLOB DIRS *)
        foreach(DIR IN LISTS DIRS)
            if(NOT DIR MATCHES ".*CMakeLists.txt")
                nw_add_subdirectory(${DIR})
            endif()
        endforeach()
    endfunction()

    if (NW_ENABLE_TESTS)
        find_package(GTest CONFIG REQUIRED)
        enable_testing()
    endif()

    function(nw_add_test NAME DIR)
        if (NW_ENABLE_TESTS)
            file(GLOB_RECURSE SRC_TEST ${DIR}/*.*)
            add_executable(${NAME} ${SRC_TEST})
            set(LIBS_V ${ARGN})
            target_link_libraries(${NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main ${LIBS_V})
            add_test(NAME ${NAME} COMMAND ${NAME})
        endif()
    endfunction()

    # Set NEWorld build output directories
    set(NEWORLD_PRODUCT_DIR ${CMAKE_BINARY_DIR}/Product)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/SDK/Lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR})
endif ()
