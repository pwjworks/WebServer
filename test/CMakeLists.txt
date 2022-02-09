add_executable(server_test server_test.cpp)
add_executable(echo_client echo_client.cpp)

add_library(echo_server echo_server.cpp)

target_link_libraries(server_test webserver_lib gtest_main echo_server)

include(GoogleTest)

gtest_discover_tests(server_test)