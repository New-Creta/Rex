// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: string_utils
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/string/character_lookup.h"
#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal::string_utils
    {
      // compares 2 strings lexicographically
      template <typename Traits, typename Pointer, typename SizeType>
      int32 compare(Pointer lhs, Pointer rhs, SizeType lhsLength, SizeType rhsLength)
      {
        return Traits::compare(lhs, rhs, rsl::min(lhsLength, rhsLength));
      }
      // finds the first substring [str, str + toFindLength) within [lhsStr, lhsStr + lhsLength), starting from pos
      template <typename Traits, typename Pointer, typename SizeType>
      SizeType find(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer toFindStr, SizeType toFindLength, SizeType defaultValue)
      {
        REX_ASSERT_X(pos < size(), "pos out of bounds");

        // substring must be found between [pos, size() - toFindLength)
        // we subtract the length of the string to find for optimization
        Pointer start         = lhsStr + pos;
        const Pointer lhs_end = lhsStr + lhsLength;

        const Pointer end = lhs_end - toFindLength + 1;
        start             = Traits::find(start, end - start, *toFindStr);

        if(start != nullptr)
        {
          while(start != end)
          {
            if(Traits::compare(start + 1, toFindStr + 1, toFindLength - 1) == 0)
            {
              return start - lhsStr;
            }
            ++start;
            start = Traits::find(start, lhsEnd - start, *toFindStr);
          }
        }
        return defaultValue;
      }
      // finds the last substring [str, str + toFindLength) within [lhsStr, lhsStr + lhsLength), starting from pos
      template <typename Traits, typename Pointer, typename SizeType>
      SizeType rfind(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer toFindStr, SizeType toFindLength, SizeType defaultValue)
      {
        pos = min(pos, lhsLength - 1);

        // the string must be found between [begin, pos]

        // we'll first check where the last char in the substring is found in the string
        Pointer start              = lhsStr + pos;
        const Pointer end          = (lhsStr - 1) + toFindLength - 1;
        const Pointer to_find_last = toFindStr + toFindLength - 1;
        start                      = Traits::rfind(start, start - end, *to_find_last);

        if(start != nullptr)
        {
          while(start != end)
          {
            const Pointer start = start - (toFindLength + 1);
            if(Traits::compare(start, toFindStr, toFindLength - 1) == 0)
            {
              return start - lhsStr;
            }
            --start;
            start = Traits::rfind(start, start - end, *to_find_last);
          }
        }
        return defaultValue;
      }
      // finds the first occurrence of a char in the substring [str, str + length) within [lhsStr, lhsStr + lhsLength), starting from pos
      template <typename Traits, typename Pointer, typename SizeType>
      SizeType find_first_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
      {
        internal::character_lookup<typename Traits::value_type> lookup(rhsStr, rhsLength);

        for(SizeType i = pos; i < lhsLength; ++i)
        {
          auto c = lhsStr[i];
          if(lookup.exists(c))
          {
            return i;
          }
        }

        return defaultValue;
      }
      // finds the last occurrence of a char in the substring [rhsStr, rhsStr + rhsLength) within [lhsStr, lhsStr + lhsLength), starting from pos
      template <typename Traits, typename Pointer, typename SizeType>
      SizeType find_last_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
      {
        internal::character_lookup<typename Traits::value_type> lookup(rhsStr, rhsLength);

        for(SizeType i = lhsLength; i >= 0; --i)
        {
          auto c = lhsStr[i];
          if(lookup.exists(c))
          {
            return i;
          }
        }

        return defaultValue;
      }
      // finds the first occurrence of a char not in the substring [str, str + length) within [lhsStr, lhsStr + lhsLength), starting from pos
      template <typename Traits, typename Pointer, typename SizeType>
      SizeType find_first_not_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
      {
        internal::character_lookup<typename Traits::value_type> lookup(rhsStr, rhsLength);

        for(SizeType i = 0; i < lhsLength; ++i)
        {
          auto c = lhsStr[i];
          if(!lookup.exists(c))
          {
            return i;
          }
        }

        return defaultValue;
      }
      // finds the last occurrence of a char not in the substring [str, str + length) within [lhsStr, lhsStr + lhsLength), starting from pos
      template <typename Traits, typename Pointer, typename SizeType>
      SizeType find_last_not_of(Pointer lhsStr, SizeType lhsLength, SizeType pos, Pointer rhsStr, SizeType rhsLength, SizeType defaultValue)
      {
        internal::character_lookup<typename Traits::value_type> lookup(rhsStr, rhsLength);

        for(SizeType i = lhsLength; i >= 0; --i)
        {
          auto c = lhsStr[i];
          if(!lookup.exists(c))
          {
            return i;
          }
        }

        return defaultValue;
      }

    } // namespace internal::string_utils
  }   // namespace v1
} // namespace rsl
