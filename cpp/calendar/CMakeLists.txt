project(calendar)

add_executable(calendar calendar.cpp)
set(EXECUTABLE_OUTPUT_PATH ${BINARY_OUTPUT_DIR})
message(STATUS "EXECUTABLE_OUTPUT_PATH: ${EXECUTABLE_OUTPUT_PATH}")
