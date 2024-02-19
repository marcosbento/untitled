//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#ifndef UNTITLED_PACKS_HPP
#define UNTITLED_PACKS_HPP

#include <cstddef>

namespace untitled {

namespace detail {

// compile time constant

template <typename T, T I>
struct constant_value {
  static constexpr T value = I;
};

// compile time constant (type size_t)

template <size_t I>
using constant_index = constant_value<size_t, I>;

// index of parameter packs

template <typename T, typename... Ts>
struct in_pack_index;

template <typename T, typename... Ts>
struct in_pack_index<T, T, Ts...> {
  static constexpr size_t value = 0;
};

template <typename T, typename U, typename... Ts>
struct in_pack_index<T, U, Ts...> {
  static constexpr size_t value = 1 + in_pack_index<T, Ts...>::value;
};

// type of parameter packs

template <size_t I, typename... Ts>
struct in_pack_type;

template <typename T, typename... Ts>
struct in_pack_type<0, T, Ts...> {
  using type = T;
};

template <size_t I, typename T, typename... Ts>
struct in_pack_type<I, T, Ts...> {
  using type = in_pack_type<I - 1, Ts...>::type;
};

// size of parameter packs

template <typename... Types>
struct in_pack_size {
  static constexpr size_t size = sizeof...(Types);
};

template <typename... Types>
constexpr size_t in_pack_size_v = in_pack_size<Types...>::size;

} // namespace detail

} // namespace untitled

#endif
