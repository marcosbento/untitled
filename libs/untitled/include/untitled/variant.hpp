//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#include <concepts>
#include <cstdlib>
#include <functional>
#include <type_traits>
#include <utility>

namespace untitled {

template <class... Types>
struct overloaded : Types... {
  using Types::operator()...;
};

namespace detail {

template <typename... Ts>
struct parameter_pack_index;

template <typename T, typename... Ts>
struct parameter_pack_index<T, T, Ts...> {
  static constexpr size_t value = 0;
};

template <typename T, typename U, typename... Ts>
struct parameter_pack_index<T, U, Ts...> {
  static constexpr size_t value = 1 + parameter_pack_index<T, Ts...>::value;
};

template <typename First, typename... Rest>
union recursive_union {
  First value_;
  recursive_union<Rest...> rest_;

  ~recursive_union() {}

  template <size_t i, typename T>
  recursive_union(std::integral_constant<size_t, i>, T&& value) : rest_(std::integral_constant<size_t, i - 1>{}, std::forward<T>(value)) {}
  template <typename T>
  recursive_union(std::integral_constant<size_t, 0>, T&& value) : value_(std::forward<T>(value)) {}

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
  recursive_union(std::integral_constant<size_t, 0>, T&& value) : value_(std::forward<T>(value)) {}

  template <size_t i>
  auto& get() {
    static_assert(0 == i);
    return value_;
  }

  template <size_t i>
  void destroy() {
    static_assert(0 == i);
    value_.~Head();
  }
};

} // namespace detail

// [variant.variant], class template variant

template <typename... Types>
class variant {
public:
  static_assert(0 < sizeof...(Types), "variant must have at least one alternative");
  static_assert((!std::is_void_v<Types> && ...), "variant cannot have array as alternative");
  static_assert((!std::is_array_v<Types> && ...), "variant cannot have array as alternative");
  static_assert((!std::is_reference_v<Types> && ...), "variant cannot have reference as alternative");

  template <typename T>
    requires(std::same_as<T, Types> || ...)
  variant(T&& value)
      : index_{detail::parameter_pack_index<T, Types...>::value},
        value_{std::integral_constant<size_t, detail::parameter_pack_index<T, Types...>::value>{}, std::forward<T>(value)} {}

  template <typename T>
  auto& get() {
    return value_.template get<detail::parameter_pack_index<T, Types...>::value>();
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
  using vtype = void (*)(Visitor&&, Variant&&);
  static constexpr vtype vfuncs[] = {[](Visitor&& visitor, Variant&& variant) { std::invoke(visitor, variant.template get<I>()); }...};
  vfuncs[variant.index()](std::forward<Visitor>(visitor), std::forward<Variant>(variant));
}

template <typename ... Types>
struct variant_size{};

template <typename ... Types>
struct variant_size<variant<Types...>> {
  static constexpr size_t size = sizeof...(Types);
};

template <typename Visitor, typename Variant>
void visit(Visitor&& visitor, Variant&& variant) {
  static constexpr size_t pack_size = variant_size<std::remove_reference_t<Variant>>::size;
  visit_at(std::make_index_sequence<pack_size>{}, std::forward<Visitor>(visitor), std::forward<Variant>(variant));
}

} // namespace untitled
