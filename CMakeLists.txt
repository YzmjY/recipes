cmake_minimum_required(VERSION 3.30)

project(recipes)

set(CMAKE_CXX_STANDARD 20)

set(CMKE_CXX_FLAGS "-O2")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)

include(FetchContent)
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0
)
FetchContent_MakeAvailable(yaml-cpp)

set(BINARY_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(splitstring)
add_subdirectory(calculator)
add_subdirectory(coroutine)
add_subdirectory(yaml)