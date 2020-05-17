cmake_minimum_required(VERSION 3.11)
project(sunrise-sunset-calculator)
enable_testing()

# CLang / GCC Config
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
endif()

# Main Source (src)
set(SOURCES
        calculator.c
        adapter.c
        )
add_library(sunrise-sunset-calculator ${SOURCES})

# Tests
add_executable(TestCalculator "calculator.c")
add_test (NAME TestCalculator COMMAND TestCalculator)
target_compile_definitions(TestCalculator PRIVATE TEST_CALCULATOR=1)

add_executable(TestAdapter "adapter.c" "calculator.c")
add_test (NAME TestAdapter COMMAND TestAdapter)
target_compile_definitions(TestAdapter PRIVATE TEST_ADAPTER=1)
