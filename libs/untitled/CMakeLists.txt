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
    include/untitled/variant.hpp
  SOURCES
    src/array.cpp
    src/variant.cpp
)

add_untitled_utest(
  NAME untitled.ut
  SOURCES
    test/array.ut.cpp
    test/variant.ut.cpp
    test/main.cpp # test driver!...
  PRIVATE_LIBS
    untitled
    Boost::boost
)
