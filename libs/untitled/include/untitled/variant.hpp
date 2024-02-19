//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#ifndef UNTITLED_VARIANT_HPP
#define UNTITLED_VARIANT_HPP

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include "untitled/packs.hpp"

namespace untitled {

template <class... Types>
struct overloaded : Types... {
  using Types::operator()...;
};

namespace detail {

template <typename First, typename... Rest>
union recursive_union {
  First value_;
  recursive_union<Rest...> rest_;

  ~recursive_union() {}

  template <size_t i, typename T>
  recursive_union(constant_index<i>, T&& value) : rest_(constant_index<i - 1>{}, std::forward<T>(value)) {}
  template <typename T>
  recursive_union(constant_index<0>, T&& value) : value_(std::forward<T>(value)) {}

  template <size_t i>
  auto& get() {
    if constexpr (0 == i) {
      return value_;
    }
    else {
      return rest_.template get<i - 1>();
    }
  }

  template <size_t i>
  void destroy() {
    if constexpr (0 == i) {
      value_.~Head();
    }
    else {
      rest_.template destroy<i - 1>();
    }
  }
};

template <typename First>
union recursive_union<First> {
  First value_;

  ~recursive_union(){};

  template <typename T>
  recursive_union(constant_index<0>, T&& value) : value_(std::forward<T>(value)) {}

  template <size_t I>
  auto& get() {
    static_assert(0 == I);
    return value_;
  }

  template <size_t I>
  void destroy() {
    static_assert(0 == I);
    value_.~Head();
  }
};

} // namespace detail

// [variant.variant], class template variant

template <typename... Types>
class variant {
public:
  static_assert(sizeof...(Types) > 0, "variant must have at least one alternative");
  static_assert((!std::is_void_v<Types> && ...), "variant cannot have array as alternative");
  static_assert((!std::is_array_v<Types> && ...), "variant cannot have array as alternative");
  static_assert((!std::is_reference_v<Types> && ...), "variant cannot have reference as alternative");

  template <typename T>
    requires(std::same_as<T, Types> || ...)
  variant(T&& value) : index_{detail::in_pack_index<T, Types...>::value},
                       value_{detail::constant_index<detail::in_pack_index<T, Types...>::value>{}, std::forward<T>(value)} {}

  template <typename T>
  auto& get() {
    return value_.template get<detail::in_pack_index<T, Types...>::value>();
  }

  template <size_t I>
  auto& get() {
    return value_.template get<I>();
  }

  constexpr auto index() const { return index_; }

private:
  size_t index_;
  detail::recursive_union<Types...> value_;
};

template <size_t... I, typename Visitor, typename Variant>
static void visit_at(std::index_sequence<I...>, Visitor&& visitor, Variant&& variant) {
  using vtype                     = void (*)(Visitor&&, Variant&&);
  static constexpr vtype vfuncs[] = {[](Visitor&& visitor, Variant&& variant) { std::invoke(visitor, variant.template get<I>()); }...};
  vfuncs[variant.index()](std::forward<Visitor>(visitor), std::forward<Variant>(variant));
}

template <typename... Types>
struct variant_size;

template <typename... Types>
struct variant_size<variant<Types...>> {
  static constexpr size_t size = detail::in_pack_size_v<Types...>;
};

template <typename Variant>
constexpr size_t variant_size_v = variant_size<Variant>::size;

template <size_t I, typename... Types>
struct variant_alternative;

template <size_t I, typename... Types>
struct variant_alternative<I, variant<Types...>> {
  using type = detail::in_pack_type<I, Types...>::type;
};

template <size_t I, typename... Types>
using variant_alternative_t = variant_alternative<I, Types...>::type;

template <typename Visitor, typename Variant>
void visit(Visitor&& visitor, Variant&& variant) {
  constexpr size_t pack_size = variant_size<std::remove_cvref_t<Variant>>::size;
  visit_at(std::make_index_sequence<pack_size>{}, std::forward<Visitor>(visitor), std::forward<Variant>(variant));
}

} // namespace untitled

#endif
