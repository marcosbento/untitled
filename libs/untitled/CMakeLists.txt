#
# Copyright (c) 2024 Marcos Bento
#
# Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
#
# See https://github.com/marcosbento/untitled
#

add_untitled_library(
  NAME untitled
  PUBLIC_INCLUDE_DIRS
    include
  PUBLIC_HEADERS
    include/untitled/array.hpp
  SOURCES
    src/array.cpp
)

add_untitled_utest(
  NAME untitled.ut
  PUBLIC_INCLUDE_DIRS
    include
  PUBLIC_HEADERS
    include/untitled/array.hpp
  SOURCES
    test/main.cpp
    test/array.ut.cpp
  PRIVATE_LIBS
    untitled
    Boost::boost
)

