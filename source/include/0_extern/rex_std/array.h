// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: array.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/fill_n.h"
#include "rex_std/internal/algorithm/lexicographical_compare.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/iterator/random_access_iterator.h"
#include "rex_std/internal/iterator/reverse_iterator.h"
#include "rex_std/internal/type_traits/conjunction.h"
#include "rex_std/internal/type_traits/is_array.h"
#include "rex_std/internal/type_traits/is_constructible.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/remove_cv.h"
#include "rex_std/internal/utility/integer_sequence.h"
#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, count_t Size>
    class array
    {
    public:
      using value_type      = T;
      using size_type       = count_t; /// RSL Comment: Different from ISO C++ Standard at time of writing (26/Jun/2022)
      using difference_type = int32;   /// RSL Comment: Different from ISO C++ Standard at time of writing (26/Jun/2022)
      using pointer         = value_type*;
      using const_pointer   = const value_type*;
      using reference       = value_type&;
      using const_reference = const value_type&;

      using iterator       = random_access_iterator<T>;
      using const_iterator = const_random_access_iterator<T>;

      using reverse_iterator       = rsl::reverse_iterator<iterator>;
      using const_reverse_iterator = rsl::reverse_iterator<const_iterator>;

      // Returns a reference to the element at specified location pos, with bounds checking.
      // if pos is not within range of the container, an assertion is raised
      constexpr reference at(size_type pos)
      {
        REX_ASSERT_X(pos < size(), "array out of bounds");
        return m_data[pos];
      }
      // Returns a reference to the element at specified location pos, with bounds checking.
      // if pos is not within range of the container, an assertion is raised
      constexpr const_reference at(size_type pos) const
      {
        REX_ASSERT_X(pos < size(), "array out of bounds");
        return m_data[pos];
      }
      // Returns a reference to the element at specified location pos. No bounds checking is performed.
      constexpr reference operator[](size_type pos)
      {
        return m_data[pos];
      }
      // Returns a reference to the element at specified location pos. No bounds checking is performed.
      constexpr const_reference operator[](size_type pos) const
      {
        return m_data[pos];
      }

      // Returns a reference to the first element in the container.
      constexpr reference front()
      {
        return m_data[0];
      }
      // Returns a reference to the first element in the container.
      constexpr const_reference front() const
      {
        return m_data[0];
      }
      // Returns a reference to the last element in the container.
      constexpr reference back()
      {
        return m_data[size() - 1];
      }
      // Returns a reference to the last element in the container.
      constexpr const_reference back() const
      {
        return m_data[size() - 1];
      }
      // Returns pointer to the underlying array serving as element storage
      constexpr T* data()
      {
        return m_data;
      }
      // Returns pointer to the underlying array serving as element storage
      constexpr const T* data() const
      {
        return m_data;
      }
      // Returns an iterator to the first element of the array.
      constexpr iterator begin()
      {
        return iterator(m_data);
      }
      // Returns an iterator to the first element of the array.
      constexpr const_iterator begin() const
      {
        return const_iterator(m_data);
      }
      // Returns an iterator to the first element of the array.
      constexpr const_iterator cbegin() const
      {
        return const_iterator(m_data);
      }
      // Returns an iterator to the element following the last element of the array.
      constexpr iterator end()
      {
        return iterator(m_data + size());
      }
      // Returns an iterator to the element following the last element of the array.
      constexpr const_iterator end() const
      {
        return const_iterator(m_data);
      }
      // Returns an iterator to the element following the last element of the array.
      constexpr const_iterator cend() const
      {
        return const_iterator(m_data + size());
      }
      // Returns a reverse iterator to the first element of the reversed array.
      // It corresponds to the last element of the non-reversed array.
      constexpr reverse_iterator rbegin()
      {
        return reverse_iterator(end());
      }
      // Returns a reverse iterator to the first element of the reversed array.
      // It corresponds to the last element of the non-reversed array.
      constexpr const_reverse_iterator rbegin() const
      {
        return const_reverse_iterator(end());
      }
      // Returns a reverse iterator to the first element of the reversed array.
      // It corresponds to the last element of the non-reversed array.
      constexpr const_reverse_iterator crbegin() const
      {
        return rbegin();
      }
      // Returns a reverse iterator to the element following the last element of the reversed array.
      // It corresponds to the element preceding the first element of the non-reversed array.
      constexpr reverse_iterator rend()
      {
        return reverse_iterator(begin());
      }
      // Returns a reverse iterator to the element following the last element of the reversed array.
      // It corresponds to the element preceding the first element of the non-reversed array.
      constexpr const_reverse_iterator rend() const
      {
        return const_reverse_iterator(begin());
      }
      // Returns a reverse iterator to the element following the last element of the reversed array.
      // It corresponds to the element preceding the first element of the non-reversed array.
      constexpr const_reverse_iterator crend() const
      {
        return rend();
      }
      // Checks if the container has no elements, i.e. whether begin() == end().
      constexpr bool empty() const
      {
        return false;
      }
      // Returns the number of elements in the container, i.e. rsl::distance(begin(), end()).
      constexpr size_type size() const
      {
        return Size;
      }
      // Returns the maximum number of elements the container is able to hold.
      constexpr size_type max_size() const
      {
        return Size;
      }

      // Assigns the given value value to all elements in the container.
      constexpr void fill(const T& val)
      {
        rsl::fill_n(m_data, size(), val);
      }
      // Exchanges the contents of the container with those of other.
      constexpr void swap(array& other)
      {
        for(card32 i = 0; i < size(); ++i)
        {
          rsl::swap(m_data[i], other[i]);
        }
      }

    public:
      // Note that the member data is intentionally public.
      // This allows for aggregate initialization of the object
      // eg. array<int, 5> arr = { 1, 2, 3, 4, 5 };
      /// RSL Comment: Different from ISO C++ Standard at time of writing (10/Oct/2022)
      T m_data[Size] = {}; // NOLINT // adding " = {}" value initializes the array
    };

    // Checks if the contents of lhs and rhs are equal
    template <typename T, count_t Size>
    constexpr bool operator==(const rsl::array<T, Size>& lhs, const rsl::array<T, Size>& rhs)
    {
      for(auto lhs_it = lhs.cbegin(), lhs_end = lhs.cend(), rhs_it = rhs.cbegin(); lhs_it != lhs_end; ++lhs_it)
      {
        if(*lhs_it != rhs_it)
        {
          return false;
        }
      }
      return true;
    }
    // Checks if the contents of lhs and rhs are equal
    template <typename T, count_t Size>
    constexpr bool operator!=(const rsl::array<T, Size>& lhs, const rsl::array<T, Size>& rhs)
    {
      return !(lhs == rhs);
    }

    // Compares the contents of lhs and rhs lexicographically
    template <typename T, count_t Size>
    constexpr bool operator<(const rsl::array<T, Size>& lhs, const rsl::array<T, Size>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) < 0;
    }
    // Compares the contents of lhs and rhs lexicographically
    template <typename T, count_t Size>
    constexpr bool operator<=(const rsl::array<T, Size>& lhs, const rsl::array<T, Size>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) <= 0;
    }
    // Compares the contents of lhs and rhs lexicographically
    template <typename T, count_t Size>
    constexpr bool operator>(const rsl::array<T, Size>& lhs, const rsl::array<T, Size>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) > 0;
    }
    // Compares the contents of lhs and rhs lexicographically
    template <typename T, count_t Size>
    constexpr bool operator>=(const rsl::array<T, Size>& lhs, const rsl::array<T, Size>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) >= 0;
    }

    // Extracts the Ith element element from the array.
    template <card32 I, typename T, count_t Size>
    constexpr T& get(rsl::array<T, Size>& a)
    {
      return a[I];
    }

    // Extracts the Ith element element from the array.
    template <card32 I, typename T, count_t Size>
    constexpr T&& get(rsl::array<T, Size>&& a)
    {
      return rsl::move(a[I]);
    }
    // Extracts the Ith element element from the array.
    template <card32 I, typename T, count_t Size>
    constexpr const T& get(const rsl::array<T, Size>& a)
    {
      return a[I];
    }

    // Extracts the Ith element element from the array.
    template <card32 I, typename T, count_t Size>
    constexpr const T&& get(const rsl::array<T, Size>&& a)
    {
      return rsl::move(a[I]);
    }

    // Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
    template <typename T, count_t Size>
    constexpr void swap(rsl::array<T, Size>& lhs, rsl::array<T, Size>& rhs)
    {
      lhs.swap(rhs);
    }

    namespace internal
    {
      // TODO: Check for the compile time overhead
      template <typename T, count_t Size, card32... I>
      constexpr rsl::array<rsl::remove_cv_t<T>, Size> to_array(T (&a)[Size]) // NOLINT
      {
        return rsl::array<remove_cv_t<T>, Size> {{a[I]...}};
      }
      template <typename T, count_t Size, card32... I>
      constexpr rsl::array<rsl::remove_cv_t<T>, Size> to_array(T (&&a)[Size]) // NOLINT
      {
        return rsl::array<remove_cv_t<T>, Size> {{rsl::move(a[I])...}};
      }
    } // namespace internal

    // Create a rsl::array from the one dimensinoal built-in array.
    // elements are copy-initialized from the corresponding element of a
    template <typename T, count_t Size>
    constexpr rsl::array<rsl::remove_cv_t<T>, Size> to_array(T (&a)[Size]) // NOLINT
    {
      static_assert(rsl::is_constructible_v<T, T&>, "element of type T must be copy constructible");
      static_assert(!rsl::is_array_v<T>, "passing multidimensional arrays to to_array is ill-formed");
      return internal::to_array(a, make_index_sequence<Size> {});
    }
    // Create a rsl::array from the one dimensinoal built-in array.
    // elements are move-initialized from the corresponding element of a
    template <typename T, count_t Size>
    constexpr rsl::array<rsl::remove_cv_t<T>, Size> to_array(T (&&a)[Size]) // NOLINT
    {
      static_assert(rsl::is_constructible_v<T, T&>, "element of type T must be copy constructible");
      static_assert(!rsl::is_array_v<T>, "passing multidimensional arrays to to_array is ill-formed");
      return internal::to_array(rsl::move(a), make_index_sequence<Size> {});
    }

    // Provides access to the number of elements in an rsl::array as a compile-time constant expression.
    template <typename T>
    class tuple_size;

    template <typename T, count_t Size>
    struct tuple_size<rsl::array<T, Size>> : rsl::integral_constant<card32, Size>
    {
    };

    template <card32 I, typename T>
    struct tuple_element;

    // Provides compile-time indexed access to the type of the elements of the array using tuple-like interface.
    template <card32 I, typename T, count_t Size>
    struct tuple_element<I, rsl::array<T, Size>>
    {
      using type = T;
    };

    // Construction of rsl::array from a variadic parameter pack.
    template <typename T, typename... U>
    array(T, U...) -> array<T, 1 + sizeof...(U)>;

  } // namespace v1
} // namespace rsl
