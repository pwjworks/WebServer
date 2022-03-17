enable_testing()

add_executable(server_test server_test.cpp)
add_executable(httpdata_test httpdata_test.cpp)

target_link_libraries(
        server_test
        #        gtest_main
        webserver
)

target_link_libraries(
        httpdata_test
        webserver
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(httpdata_test)