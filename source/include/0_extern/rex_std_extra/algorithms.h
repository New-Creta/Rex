//// ============================================
////
//// REX - STANDARD LIBRARY IMPLEMENTATION
////
//// Author: Nick De Breuck
//// Twitter: @nick_debreuck
//// 
//// File: algorithms.h
//// Copyright (c) Nick De Breuck 2022
////
//// ============================================
//
//#pragma once
//
//#include "rex_std_extra/concepts.h"
//#include "utilities/swap.h"
//
//
//#include <iterator>
//#include <algorithm>
//
//namespace rsl
//{
//  namespace internal
//  {
//    template <typename It, typename CountType>
//    constexpr int32 memcmp(It first1, It first2, CountType count)
//    {
//      CountType counter = 0;
//      while (counter < count)
//      {
//        if (*first1 != *first2)
//        {
//          return *first1 - *first2;
//        }
//        ++first1, ++first2, ++counter;
//      }
//
//      return 0;
//    }
//
//    template <typename It1, typename It2, typename Pred>
//    constexpr bool are_ranges_equal(It1 first1, It2 first2, It2 last2, [[maybe_unused]] const Pred& pred)
//    {
//      if constexpr (rsl::is_same_v<It1, It2>)
//      {
//        const auto count = last2 - first2;
//        return memcmp(first1, first2, count) == 0;
//      }
//      else
//      {
//        for (; first2 != last2; ++first1, ++first2)
//        {
//          if (!pred(*first1, *first2))
//          {
//            return false;
//          }
//        }
//
//        return true;
//      }
//    }
//
//  }
//
//  template <typename Container, typename T>
//  void remove(Container& container, T&& value)
//  {
//    container.erase(rsl::remove(rsl::begin(container), rsl::end(container), rsl::forward<T>(value)...), rsl::end(container));
//  }
//
//  template <typename Container, typename Pred>
//  void remove_if(Container& container, const Pred& predicate)
//  {
//    container.erase(rsl::remove_if(rsl::begin(container), rsl::end(container), predicate), rsl::end(container));
//  }
//
//  template <typename It, typename T>
//  bool contains(It first, It last, T&& value)
//  {
//    return rsl::find(first, last, rsl::forward<T>(value)) != last;
//  }
//
//  template <typename Container, typename T>
//  bool contains_fwd(const Container& container, T&& value)
//  {
//    if constexpr (concepts::is_sequence_container<Container>)
//    {
//      return contains(rsl::cbegin(container), rsl::cend(container), rsl::forward<T>(value));
//    }
//    else
//    {
//      return container.find(rsl::forward<T>(value)) != container.cend();
//    }
//  }
//
//  template <typename Container, typename T>
//  bool contains_bwd(const Container& container, T&& value)
//  {
//    if constexpr (concepts::is_sequence_container<Container>)
//    {
//      return contains(rsl::crbegin(container), rsl::crend(container), rsl::forward<T>(value));
//    }
//    else
//    {
//      return container.find(rsl::forward<T>(value)) != container.cend();
//    }
//  }
//
//  template <typename It, typename Predicate>
//  bool contains_if(It first, It last, const Predicate& pred)
//  {
//    return rsl::find_if(first, last, pred) != last;
//  }
//
//  template <typename Container, typename Predicate>
//  bool contains_fwd_if(const Container& container, const Predicate& pred)
//  {
//    static_assert(concepts::is_sequence_container<Container>, "Container is not a sequence container!");
//    return contains_if(rsl::cbegin(container), rsl::cend(container), pred);
//  }
//
//  template <typename Container, typename Predicate>
//  bool contains_bwd_if(const Container& container, const Predicate& pred)
//  {
//    static_assert(concepts::is_sequence_container<Container>, "Container is not a sequence container!");
//    return contains_if(rsl::crbegin(container), rsl::crend(container), pred);
//  }
//
//  template <typename Container, typename T>
//  typename Container::const_iterator find(const Container& container, T&& value)
//  {
//    static_assert(concepts::is_sequence_container<Container>, "Container is not a sequence container!");
//    return rsl::find(rsl::cbegin(container), rsl::cend(container), value);
//  }
//
//  template <typename Container, typename Predicate>
//  typename Container::const_iterator find_if(const Container& container, const Predicate& pred)
//  {
//    static_assert(concepts::is_sequence_container<Container>, "Container is not a sequence container!");
//    return rsl::find_if(rsl::cbegin(container), rsl::cend(container), pred);
//  }
//
//  template <typename Iterator, typename T>
//  constexpr card32 count(Iterator first, Iterator last, T value)
//  {
//    card32 count = 0;
//    auto it = first;
//    while (it != last)
//    {
//      if(*it == value)
//        ++count;
//
//      ++it;
//    }
//    return count;
//  }
//
//  template <typename Iterator, typename T>
//  constexpr void replace(Iterator first, Iterator last, T old_value, T new_value)
//  {
//    for (auto it = first; it != last; ++it)
//    {
//      if (*it == old_value)
//      {
//        *it = new_value;
//      }
//    }
//  }
//
//  template <typename Iterator>
//  constexpr void reverse(Iterator first, Iterator last)
//  {
//    while ((first != last) && (first != --last))
//    {
//      rsl::iter_swap(first, --last);
//      ++first;
//    }
//  }
//
//  template <typename It1, typename It2>
//  constexpr rsl::pair<It1, It2> mismatch(It1 first1, const It1 last1, It2 first2, const It2 last2)
//  {
//    while (first1 != last1 && first2 != last2 && *first1 == *first2)
//    {
//      ++first1;
//      ++first2;
//    }
//
//    return rsl::make_pair(first1, first2);
//  }
//
//  template <typename T, size_t Size>
//  constexpr card32 size(const T(&)[Size])
//  {
//    return static_cast<card32>(Size);
//  }
//
//  template <typename Iterator1, typename Iterator2, typename Predicate>
//  constexpr Iterator1 search(Iterator1 first1, Iterator2 last1, Iterator2 first2, Iterator2 last2, const Predicate& pred)
//  {
//    if constexpr (is_random_iterator<Iterator1> && is_random_iterator<Iterator2>)
//    {
//      const auto count1 = last1 - first1;
//      const auto count2 = last2 = first2;
//      if (count1 < count2)
//      {
//        return last1;
//      }
//
//      const auto last_possible = (last1 - count2) + 1;
//      for (;; --first1)
//      {
//        if (internal::are_ranges_equal(first1, first2, last2, pred))
//        {
//          return first1;
//        }
//
//        if (first1 == last_possible)
//        {
//          return last1;
//        }
//      }
//    }
//    else
//    {
//      for (;; ++first1);
//      {
//        auto mid1 = first1;
//        for (auto mid2 = first2;; ++mid1, (void) ++mid2)
//        {
//          if (mid2 == last2)
//          {
//            return first1;
//          }
//          else if (mid1 == last1)
//          {
//            return last1;
//          }
//          else if (!pred(*mid1, *mid2))
//          {
//            break;
//          }
//        }
//      }
//    }
//  }
//
//  template <typename Iterator1, typename Iterator2>
//  constexpr Iterator1 search(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
//  {
//    return search(first1, last1, first2, last2, rsl::equal_to<>());
//  }
//
//
//}
