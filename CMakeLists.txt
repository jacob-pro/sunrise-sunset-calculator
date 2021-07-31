cmake_minimum_required(VERSION 3.11)
project(sunrise-sunset-calculator)
enable_testing()
include(CheckLibraryExists)
set(CMAKE_C_STANDARD 99)
include_directories(tinytest)
include_directories(include)
include_directories(src)

# Math library
check_library_exists(m sin "" HAVE_LIB_M)                                                                                                                                                                                                                                         
if (HAVE_LIB_M)                                                                                                                          
 set(EXTRA_LIBS ${EXTRA_LIBS} m)                                                                                                      
endif()

# Enable all warnings
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Werror")
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
endif()

# The library
set(SOURCES
        src/spa.c
        src/ssc.c
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

# Tests
add_executable(test_spa "src/spa.c" "test/spa_tester.c")
target_link_libraries(test_spa PUBLIC ${EXTRA_LIBS})
add_test (NAME test_spa COMMAND test_spa)

add_executable(test_ssc "src/spa.c" "src/ssc.c" "test/test_ssc.c")
target_link_libraries(test_ssc PUBLIC ${EXTRA_LIBS})
add_test(NAME test_ssc COMMAND test_ssc)

# Demo Apps
add_executable(example "src/spa.c" "src/ssc.c" "test/ssc_example.c")
target_link_libraries(example PUBLIC ${EXTRA_LIBS})

# Code format helper tools
file(GLOB ALL_C_SOURCE_FILES
        include/*.[chi]
        src/*.[chi]
        test/*.[chi]
        )
add_custom_target(clang-format COMMAND clang-format-12 --style=file -i ${ALL_C_SOURCE_FILES})
