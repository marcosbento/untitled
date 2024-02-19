//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#include "untitled/expected.hpp"

#include <expected>
#include <string>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(t_untitled)
BOOST_AUTO_TEST_SUITE(expected)

BOOST_AUTO_TEST_CASE(can_create_unexpected) {
  untitled::unexpected u{42};
  BOOST_CHECK_EQUAL(u.error(), 42);
}

BOOST_AUTO_TEST_CASE(can_create_expected_from_value) {
  auto value = std::string{"hola!"};
  untitled::expected<std::string, int> r(value);
  BOOST_CHECK_EQUAL(r.value(), value);
}

BOOST_AUTO_TEST_CASE(can_create_expected_from_error) {
  auto error = 42;
  untitled::expected<std::string, int> r(untitled::unexpected{error});
  BOOST_CHECK_EQUAL(r.error(), error);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
