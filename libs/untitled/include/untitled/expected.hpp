//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#ifndef UNTITLED_EXPECTED_HPP
#define UNTITLED_EXPECTED_HPP

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include "untitled/variant.hpp"

namespace untitled {

template <typename E>
struct unexpected {
  unexpected(const unexpected&) = default;
  unexpected(unexpected&&)      = default;

  template <typename EE = E>
    requires std::is_same_v<std::remove_cvref_t<EE>, E>
  unexpected(EE&& e) : error_{std::forward<EE>(e)} {}

  constexpr const E& error() const& { return error_; }
  constexpr E& error() & { return error_; }

private:
  E error_;
};

template <class E>
unexpected(E) -> unexpected<E>;

template <typename V, typename E>
class expected {
public:
  static_assert((!std::is_void_v<V>), "expected value can not be void");
  static_assert((!std::is_void_v<E>), "expected error can not be void");
  static_assert((!std::is_same_v<V, E>), "expected value and error cannot be of the same type");
  static_assert((!std::is_reference_v<V>), "expected value can not be reference");
  static_assert((!std::is_reference_v<E>), "expected error can not be reference");

  using value_type      = V;
  using error_type      = E;
  using unexpected_type = unexpected<E>;

  expected(value_type v) : stored_{std::move(v)} {}

  template <typename T>
    requires std::is_same_v<T, V>
  expected(T&& u) : stored_{std::forward<T>(u)} {}

  template <typename T>
    requires std::is_same_v<std::remove_cvref_t<T>, V>
  expected(T&& u) : stored_{std::forward<T>(u)} {}

  template <typename U>
    requires std::is_same_v<U, unexpected<E>>
  expected(U&& u) : stored_{std::forward<U>(u)} {}

  auto&& value() { return stored_.template get<0>(); }
  auto&& error() { return stored_.template get<1>().error(); }

private:
  variant<V, unexpected<E>> stored_;
};

} // namespace untitled

#endif
