#enable_testing()

add_executable(server_test server_test.cpp)
add_executable(echo_client echo_client.cpp)

target_link_libraries(
        server_test
        #        gtest_main
        webserver
)

#include(GoogleTest)
#gtest_discover_tests(server_test)