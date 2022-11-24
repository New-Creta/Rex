// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: copy.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/type_traits/is_move_iterator.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/iterator/iterator_traits.h"
#include "rex_std/internal/memory/memmove.h"
#include "rex_std/internal/type_traits/is_pointer.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/is_trivially_copyable.h"
#include "rex_std/internal/utility/move.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename InputIteratorCategory, bool IsMove, bool CanMemMove>
      struct move_and_copy_helper
      {
        template <typename InputIterator, typename OutputIterator>
        constexpr static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator dstFirst)
        {
          for(; first != last; ++dstFirst, ++first)
          {
            *dstFirst = *first;
          }

          return dstFirst;
        }
      };

      template <>
      struct move_and_copy_helper<rsl::random_access_iterator_tag, false, false>
      {
        template <typename InputIterator, typename OutputIterator>
        constexpr static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator result)
        {
          using difference_type = typename iterator_traits<InputIterator>::difference_type;

          for(difference_type n = (last - first); n > 0; --n, ++first, ++result)
          {
            *result = *first;
          }

          return result;
        }
      };

      template <typename InputIteratorCategory>
      struct move_and_copy_helper<InputIteratorCategory, true, false>
      {
        template <typename InputIterator, typename OutputIterator>
        constexpr static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator result)
        {
          for(; first != last; ++result, ++first)
          {
            *result = rsl::move(*first);
          }
          return result;
        }
      };

      template <>
      struct move_and_copy_helper<rsl::random_access_iterator_tag, true, false>
      {
        template <typename InputIterator, typename OutputIterator>
        constexpr static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator result)
        {
          using difference_type = typename InputIterator::difference_type;

          for(difference_type n = (last - first); n > 0; --n, ++first, ++result)
          {
            *result = rsl::move(*first);
          }

          return result;
        }
      };

      template <bool IsMove>
      struct move_and_copy_helper<rsl::random_access_iterator_tag, IsMove, true>
      {
        template <typename T>
        constexpr static T* move_or_copy(const T* first, const T* last, T* result)
        {
          if(first == last)
            return result;

          // We could use memcpy here if there's no range overlap, but memcpy is rarely much faster than memmove.
          const card32 len = static_cast<card32>(reinterpret_cast<uintptr>(last) - reinterpret_cast<uintptr>(first)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
          return static_cast<T*>(memmove(result, first, len)) + len;
        }
      };
    } // namespace internal

    template <typename InputIt, typename OutputIt>
    constexpr OutputIt copy(InputIt first, InputIt last, OutputIt dstFirst)
    {
      using IIC               = typename iterator_traits<InputIt>::iterator_category;
      using OIC               = typename iterator_traits<OutputIt>::iterator_category;
      using value_type_input  = typename iterator_traits<InputIt>::value_type;
      using value_type_output = typename iterator_traits<OutputIt>::value_type;

      constexpr bool is_move         = rsl::is_move_iterator_v<InputIt>;
      constexpr bool can_be_memmoved = rsl::is_trivially_copyable_v<value_type_output> && rsl::is_same_v<value_type_input, value_type_output> &&
                                       (rsl::is_pointer_v<InputIt> || rsl::is_same_v<IIC, rsl::continuous_iterator_tag>)&&(rsl::is_pointer_v<OutputIt> || rsl::is_same_v<OIC, rsl::continuous_iterator_tag>);

      return internal::move_and_copy_helper<IIC, is_move, can_be_memmoved>::move_or_copy(first, last, dstFirst);
    }

  } // namespace v1
} // namespace rsl
