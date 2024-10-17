project(hanoi)

add_executable(hanoi hanoi.cpp)
set(EXECUTABLE_OUTPUT_PATH ${BINARY_OUTPUT_DIR})
message(STATUS "EXECUTABLE_OUTPUT_PATH: ${EXECUTABLE_OUTPUT_PATH}")

target_link_libraries(hanoi spdlog)