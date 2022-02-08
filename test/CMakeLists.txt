add_executable(server_test server_test.cpp)

target_link_libraries(server_test webserver_lib gtest_main)

include(GoogleTest)

gtest_discover_tests(server_test)