enable_testing()

file(GLOB_RECURSE CRT_TEST Crt/*.*)
add_executable(NRT.Tests.Crt ${CRT_TEST})
target_link_libraries(NRT.Tests.Crt NRT.CRT gtest_main)
add_test(NAME NRT.Tests.Crt COMMAND NRT.Tests.Crt)

file(GLOB_RECURSE CFX_TEST Cfx/*.*)
add_executable(NRT.Tests.Cfx ${CFX_TEST})
target_link_libraries(NRT.Tests.Cfx NRT.CFX gtest_main)
add_test(NAME NRT.Tests.Cfx COMMAND NRT.Tests.Cfx)

file(GLOB_RECURSE CONC_TEST Conc/*.*)
add_executable(NRT.Tests.Conc ${CONC_TEST})
target_link_libraries(NRT.Tests.Conc NRT.Conc gtest_main)
add_test(NAME NRT.Tests.Conc COMMAND NRT.Tests.Conc)

file(GLOB_RECURSE MSG_TEST Msg/*.*)
add_executable(NRT.Tests.Msg ${MSG_TEST})
target_link_libraries(NRT.Tests.Msg NRT.Msg gtest_main)
add_test(NAME NRT.Tests.Msg COMMAND NRT.Tests.Msg)
