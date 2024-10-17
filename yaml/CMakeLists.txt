project(yaml-test)

add_executable(yaml-test main.cpp)
set(EXECUTABLE_OUTPUT_PATH ${BINARY_OUTPUT_DIR})
message(STATUS "EXECUTABLE_OUTPUT_PATH: ${EXECUTABLE_OUTPUT_PATH}")

target_link_libraries(yaml-test spdlog yaml-cpp)