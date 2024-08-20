project(calculator)

add_executable(calculator calculator.cpp)
set(EXECUTABLE_OUTPUT_PATH ${BINARY_OUTPUT_DIR})
message(STATUS "EXECUTABLE_OUTPUT_PATH: ${EXECUTABLE_OUTPUT_PATH}")
