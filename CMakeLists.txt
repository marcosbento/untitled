#
# Copyright (c) 2024 Marcos Bento
#
# Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
#
# See https://github.com/marcosbento/untitled
#

cmake_minimum_required(VERSION 3.26)

project(untitled
  VERSION 0.0.1
  HOMEPAGE_URL https://github.com/marcosbento/untitled
  LANGUAGES CXX
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(BuildRecipes)
include(Dependencies)

add_subdirectory(libs)
