cmake_minimum_required(VERSION 3.14)

# Actually, we want asio
find_package(Boost COMPONENTS system)

if (Boost_FOUND)
    file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.*)
    add_library(NRT.Msg SHARED ${SOURCE})
    target_enable_ipo(NRT.Msg)
    target_compile_definitions(NRT.Msg PRIVATE -DNRTMSG_BUILD)
    target_include_directories(NRT.Msg PRIVATE ${Boost_INCLUDE_DIRS})
    target_include_directories(NRT.Msg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
    target_include_directories(NRT.Msg INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
    target_link_libraries(NRT.Msg PUBLIC NRT.Core)
	target_link_libraries(NRT.Msg PRIVATE Boost::system)
else()
    message("Boost not found, Skip building concurrent support")
endif()
