// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: concepts.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/void.h"
#include "rex_std/internal/utility/declval.h"

namespace rsl
{
  template <typename T, template <typename> typename expression, typename = rsl::void_t<>>
  struct implements : rsl::false_type {};

  template <typename T, template <typename> typename expression>
  struct implements<T, expression, rsl::void_t<expression<T>>> : rsl::TrueType{};

  template <typename T, template <typename> typename expression>
  constexpr bool implements_v = implements<T, expression>::value;

  namespace implement_expressions
  {
    template <typename T>
    using begin_function = decltype(rsl::declval<T>().begin());

    template <typename T>
    using end_function = decltype(rsl::declval<T>().end());

    template <typename T>
    using size_function = decltype(rsl::declval<T>().size());

    template <typename T>
    using reserve_function = decltype(rsl::declval<T>().reserve(rsl::declval<typename T::size_type>()));

    template <typename T>
    using push_back_function = decltype(rsl::declval<T>().find(rsl::declval<typename T::value_type>()));

    template <typename T>
    using find_function = decltype(rsl::declval<T>().find(rsl::declval<typename T::key_type>()));

    template <typename T>
    using value_type = typename T::value_type;

    template <typename T>
    using underlying_type = typename T::underlying_type;

    template <typename T>
    using iterator = typename T::iterator;

    template <typename T>
    using const_iterator = typename T::const_iterator;

    template <typename T>
    using to_string_func = decltype(rsl::declval<T>().to_string());
  }

  namespace concepts
  {
    template <typename T>
    constexpr bool is_container_v =
      implements_v<T, implement_expressions::begin_function>
      && implements_v<T, implement_expressions::end_function>
      && implements_v<T, implement_expressions::size_function>
      && implements_v<T, implement_expressions::reserve_function>
      && implements_v<T, implement_expressions::value_type>;

    template <typename T>
    constexpr bool is_sequence_container =
      is_container_v<T>
      && implements_v<T, implement_expressions::find_function> == false;

    template <typename T>
    constexpr bool is_associative_container =
      is_container_v<T>
      && implements_v<T, implement_expressions::find_function> == true;
  }

  template <typename Iter>
  using IteratoryCategory = typename Iter::iterator_category;

  template <typename T>
  constexpr bool is_input_iterator = rsl::IsConvertible<IteratoryCategory<T>, rsl::input_iterator_tag>;

  template <typename T>
  constexpr bool is_random_iterator = rsl::IsConvertible<IteratoryCategory<T>, rsl::random_access_iterator_tag>;

  template <typename T>
  constexpr bool is_bidirection_iterator = rsl::IsConvertible<IteratorCategory<T>, rsl::bidirectional_iterator_tag>;

  template <typename T>
  constexpr bool is_forwward_iterator = rsl::IsConvertible<IteratoryCategory<T>, rsl::forward_iterator_tag>;
}