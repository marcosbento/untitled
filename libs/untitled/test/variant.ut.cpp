//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#include "untitled/variant.hpp"

#include <string>

#include <boost/test/unit_test.hpp>

template <typename... Ts>
struct make_recursive_union {
  template <typename T>
  static auto with(T&& v) {
    return untitled::detail::recursive_union<Ts...>(untitled::detail::constant_index<untitled::detail::in_pack_index<T, Ts...>::value>{}, std::forward<T>(v));
  }
};

BOOST_AUTO_TEST_SUITE(t_untitled)
BOOST_AUTO_TEST_SUITE(variant)

BOOST_AUTO_TEST_CASE(can_get_index_of_parameter_in_pack) {
  static_assert(0 == untitled::detail::in_pack_index<int, int, double, char>::value);
  static_assert(1 == untitled::detail::in_pack_index<int, double, int, char>::value);
  static_assert(2 == untitled::detail::in_pack_index<int, double, char, int>::value);
}

BOOST_AUTO_TEST_CASE(can_get_type_of_parameter_in_pack) {
  static_assert(std::is_same_v<int, untitled::detail::in_pack_type<0, int, double, char, std::string>::type>);
  static_assert(std::is_same_v<double, untitled::detail::in_pack_type<1, int, double, char, std::string>::type>);
  static_assert(std::is_same_v<char, untitled::detail::in_pack_type<2, int, double, char, std::string>::type>);
  static_assert(std::is_same_v<std::string, untitled::detail::in_pack_type<3, int, double, char, std::string>::type>);
}

BOOST_AUTO_TEST_CASE(can_create_and_access_union) {
  auto u1 = make_recursive_union<int, double, char>::with(42);
  BOOST_CHECK_EQUAL(u1.get<0>(), 42);
  auto u2 = make_recursive_union<int, double, char>::with('a');
  BOOST_CHECK_EQUAL(u2.get<2>(), 'a');
  auto u3 = make_recursive_union<int, double, char>::with(3.1415);
  BOOST_CHECK_CLOSE(u3.get<1>(), 3.1415, 1e-15);
}

BOOST_AUTO_TEST_CASE(can_create_variant) {
  untitled::variant<int, double, char, std::string> vs = std::string("hola!");
  untitled::variant<int, double, char, std::string> vc = 'a';
  untitled::variant<int, double, char, std::string> vd = 3.1415;
  untitled::variant<int, double, char, std::string> vi = 42;

  BOOST_CHECK_EQUAL(vs.get<std::string>(), vs.get<3>());
  BOOST_CHECK_EQUAL(vc.get<char>(), vc.get<2>());
  BOOST_CHECK_EQUAL(vd.get<double>(), vd.get<1>());
  BOOST_CHECK_EQUAL(vi.get<int>(), vi.get<0>());
}

BOOST_AUTO_TEST_CASE(can_get_variant_size) {
  using v4 = untitled::variant<int, double, char, std::string>;
  using v3 = untitled::variant<int, double, char>;
  using v2 = untitled::variant<int, double>;
  using v1 = untitled::variant<int>;

  static_assert(untitled::variant_size<v4>::size == 4);
  static_assert(untitled::variant_size<v3>::size == 3);
  static_assert(untitled::variant_size<v2>::size == 2);
  static_assert(untitled::variant_size<v1>::size == 1);

  static_assert(untitled::variant_size_v<v4> == 4);
  static_assert(untitled::variant_size_v<v3> == 3);
  static_assert(untitled::variant_size_v<v2> == 2);
  static_assert(untitled::variant_size_v<v1> == 1);
}

BOOST_AUTO_TEST_CASE(can_get_variant_type) {
  using v = untitled::variant<int, double, char, std::string>;

  static_assert(std::is_same_v<untitled::variant_alternative<0, v>::type, int>);
  static_assert(std::is_same_v<untitled::variant_alternative<1, v>::type, double>);
  static_assert(std::is_same_v<untitled::variant_alternative<2, v>::type, char>);
  static_assert(std::is_same_v<untitled::variant_alternative<3, v>::type, std::string>);

  static_assert(std::is_same_v<untitled::variant_alternative_t<0, v>, int>);
  static_assert(std::is_same_v<untitled::variant_alternative_t<1, v>, double>);
  static_assert(std::is_same_v<untitled::variant_alternative_t<2, v>, char>);
  static_assert(std::is_same_v<untitled::variant_alternative_t<3, v>, std::string>);
}

BOOST_AUTO_TEST_CASE(can_visit_variant) {
  untitled::variant<int, double, char, std::string> vs = std::string("hola!");
  untitled::variant<int, double, char, std::string> vc = 'a';
  untitled::variant<int, double, char, std::string> vd = 3.1415;
  untitled::variant<int, double, char, std::string> vi = 42;

  auto visitor = untitled::overloaded{[](const std::string& s) {
                                        BOOST_CHECK_EQUAL(s, std::string("hola!"));
                                        std::cout << "type matched: string\n";
                                        return s;
                                      },
                                      [](char c) {
                                        BOOST_CHECK_EQUAL(c, 'a');
                                        std::cout << "type matched: char\n";
                                        return c;
                                      },
                                      [](double d) {
                                        BOOST_CHECK_EQUAL(d, 3.1415);
                                        std::cout << "type matched: double\n";
                                        return d;
                                      },
                                      [](int i) {
                                        BOOST_CHECK_EQUAL(i, 42);
                                        std::cout << "type matched: int\n";
                                        return i;
                                      }};

  untitled::visit(visitor, vs);
  untitled::visit(visitor, vc);
  untitled::visit(visitor, vd);
  untitled::visit(visitor, vi);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
