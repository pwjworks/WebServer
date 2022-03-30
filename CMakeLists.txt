cmake_minimum_required(VERSION 3.16)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE "Debug")
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(./src/server ./test ./src/epoller ./src/http ./src/util ./src/eventloop ./src/buffer
        )

add_subdirectory(src)
add_subdirectory(test)