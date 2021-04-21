cmake_minimum_required(VERSION 3.11)
project(sunrise-sunset-calculator)
enable_testing()
include(CheckLibraryExists)

# Math library
check_library_exists(m sin "" HAVE_LIB_M)                                                                                                                                                                                                                                         
if (HAVE_LIB_M)                                                                                                                          
 set(EXTRA_LIBS ${EXTRA_LIBS} m)                                                                                                      
endif()

# CLang / GCC Config
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
endif()

# The library
set(SOURCES
        calculator.c
        adapter.c
        brightness.c
        )
add_library(ssc ${SOURCES})
target_link_libraries(ssc PUBLIC ${EXTRA_LIBS})

set(TYPES ${CMAKE_CONFIGURATION_TYPES})
list(TRANSFORM TYPES PREPEND _)
list(APPEND TYPES "")
foreach( OUTPUTCONFIG ${TYPES} )
 string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
 set_target_properties(ssc PROPERTIES RUNTIME_OUTPUT_DIRECTORY${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )
 set_target_properties(ssc PROPERTIES LIBRARY_OUTPUT_DIRECTORY${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
 set_target_properties(ssc PROPERTIES ARCHIVE_OUTPUT_DIRECTORY${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
endforeach( OUTPUTCONFIG TYPES )


# Unit Tests
add_executable(TestCalculator "calculator.c")
target_link_libraries(TestCalculator PUBLIC ${EXTRA_LIBS})
add_test (NAME TestCalculator COMMAND TestCalculator)
target_compile_definitions(TestCalculator PRIVATE TEST_CALCULATOR=1)

add_executable(TestAdapter "adapter.c" "calculator.c")
target_link_libraries(TestAdapter PUBLIC ${EXTRA_LIBS})
add_test (NAME TestAdapter COMMAND TestAdapter)
target_compile_definitions(TestAdapter PRIVATE TEST_ADAPTER=1)

add_executable(TestBrightness "adapter.c" "calculator.c" "brightness.c")
target_link_libraries(TestBrightness PUBLIC ${EXTRA_LIBS})
add_test (NAME TestBrightness COMMAND TestBrightness)
target_compile_definitions(TestBrightness PRIVATE TEST_BRIGHTNESS=1)

# Demo Executable
add_executable(BrightnessSimulation "adapter.c" "calculator.c" "brightness.c")
target_link_libraries(BrightnessSimulation PUBLIC ${EXTRA_LIBS})
target_compile_definitions(BrightnessSimulation PRIVATE BRIGHTNESS_SIM=1)


