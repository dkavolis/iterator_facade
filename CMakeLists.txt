cmake_minimum_required(VERSION 3.15)

# Generate compile_commands.json for clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ITERF_ENABLE_UNIT_TESTING
       "Enable unit tests for the projects (from the `test` subfolder)." OFF)

option(ITERF_ENABLE_DOXYGEN "Enable Doxygen documentation builds of source."
       OFF)

if(ITERF_ENABLE_UNIT_TESTING)
  list(APPEND VCPKG_MANIFEST_FEATURES "test")
endif()

#
# Project details
#

project(
  iterator_facade
  VERSION 0.1.0 # <<COOKIETEMPLE_FORCE_BUMP>>
  LANGUAGES CXX)

#
# Prevent building in the source directory
#

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n"
  )
endif()

#
# Create library, setup header and source files
#

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#
# Enable Doxygen
#

if(ITERF_ENABLE_DOXYGEN)
  set(DOXYGEN_CALLER_GRAPH YES)
  set(DOXYGEN_CALL_GRAPH YES)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_PREDEFINED ITERF_DOXYGEN_RUNNING)
  set(DOXYGEN_ENABLE_PREPROCESSING YES)
  set(DOXYGEN_GENERATE_XML YES)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/docs)

  find_package(Doxygen REQUIRED dot)
  doxygen_add_docs(doxygen-docs ${CMAKE_CURRENT_LIST_DIR}/include)
endif()

#
# Install library for easy downstream inclusion
#

include(GNUInstallDirs)
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES
  DESTINATION include
  PUBLIC_HEADER DESTINATION include)

install(
  EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

#
# Install the `include` directory
#

install(DIRECTORY include/${PROJECT_NAME_LOWERCASE} DESTINATION include)

#
# Quick `ConfigVersion.cmake` creation
#

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

message(STATUS "Finished building requirements for installing the package.\n")

#
# Unit testing setup
#

if(ITERF_ENABLE_UNIT_TESTING)
  enable_testing()
  message(
    STATUS
      "Build unit tests for the project. Tests should always be found in the test folder\n"
  )
  add_subdirectory(test)
endif()
