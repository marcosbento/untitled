#
# Copyright (c) 2024 Marcos Bento
#
# Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
#
# See https://github.com/marcosbento/untitled
#

# Language Configuration

set(CMAKE CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)


# Project Configuration

enable_testing()


# Build Options

option(UNTITLED_BUILD_SHARED_LIBS "Select type of libraries to build (OFF, meaning STATIC, by default)" OFF)
set(BUILD_SHARED_LIBS ${UNTITLED_BUILD_SHARED_LIBS})

option(UNTITLED_EXPORT_COMPILE_COMMANDS "Export Compilation Commands database (ON, by default)" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ${UNTITLED_EXPORT_COMPILE_COMMANDS})


# Build Recipes

function(add_untitled_library)

  set(options "")
  set(oneValueArgs 
    NAME ALIAS OUTPUT_NAME)
  set(multiValueArgs 
    SOURCES PUBLIC_HEADERS PRIVATE_HEADERS PUBLIC_INCLUDE_DIRS PRIVATE_INCLUDE_DIRS PUBLIC_LIBS PRIVATE_LIBS DEFINITIONS)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_library(${ARGS_NAME})

  if(ARGS_ALIAS)
    add_library(${ARGS_ALIAS} ALIAS ${ARGS_NAME})
  endif()

  target_sources(${ARGS_NAME}
    PUBLIC
    FILE_SET public_includes
    TYPE HEADERS
    BASE_DIRS
      include
      ${CMAKE_BINARY_DIR}/generated
    FILES
      ${ARGS_PUBLIC_HEADERS})

  target_sources(${ARGS_NAME}
    PRIVATE
      ${ARGS_PRIVATE_HEADERS}
      ${ARGS_SOURCES})

  target_compile_definitions(${ARGS_NAME}
    PRIVATE
      ${ARGS_DEFINITIONS})

  target_include_directories(${ARGS_NAME}
    PUBLIC
      ${ARGS_PUBLIC_INCLUDE_DIRS})

  target_include_directories(${ARGS_NAME}
    PUBLIC
      ${ARGS_PRIVATE_INCLUDE_DIRS})

  target_link_libraries(${ARGS_NAME}
    PUBLIC
      ${ARGS_PUBLIC_LIBS}
    PRIVATE
      ${ARGS_PRIVATE_LIBS})

  set_target_properties(${ARGS_NAME}
    PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

  if(ARGS_OUTPUT_NAME)
    set_target_properties(${ARGS_NAME}
      PROPERTIES
      OUTPUT_NAME "${ARGS_OUTPUT_NAME}")
  endif()

  install(
    TARGETS ${ARGS_NAME}
    FILE_SET public_includes)

endfunction()

function(add_untitled_utest )

  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES HEADERS INCLUDE_DIRS PUBLIC_LIBS PRIVATE_LIBS LIBRARY_DIRS DEFINITIONS)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_executable(${ARGS_NAME})

  target_sources(${ARGS_NAME}
    PRIVATE
      ${ARGS_HEADERS}
      ${ARGS_SOURCES})

  target_include_directories(${ARGS_NAME}
    PRIVATE
      ${ARGS_INCLUDE_DIRS})

  target_compile_definitions(${ARGS_NAME}
    PUBLIC
      ${ARGS_DEFINITIONS})

  target_link_libraries(${ARGS_NAME}
    PUBLIC
      ${ARGS_PUBLIC_LIBS}
    PRIVATE
      ${ARGS_PRIVATE_LIBS})

  set_target_properties(${ARGS_NAME}
    PROPERTIES
      POSITION_INDEPENDENT_CODE ON
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test")

  add_test(
    NAME ${ARGS_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${ARGS_NAME})

endfunction()
