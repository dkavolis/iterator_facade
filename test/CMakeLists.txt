cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(${CMAKE_PROJECT_NAME}_tests LANGUAGES CXX)

#
# Set the sources for the unit tests and add the executable(s)
#

set(test_sources src/main.cpp src/iterator.cpp)
add_executable(${PROJECT_NAME} ${test_sources})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # default depth is fairly useless for debugging
  target_compile_options(${PROJECT_NAME} PRIVATE -fconcepts-diagnostics-depth=3)
endif()

find_package(Catch2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Catch2::Catch2
                                             iterator_facade::iterator_facade)

include(../cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME} True)

#
# Add the unit tests
#

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
