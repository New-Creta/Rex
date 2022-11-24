// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: red_black_tree_base.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/is_empty.h"
#include "rex_std/internal/utility/pair.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Compare, bool /*bIsEmpty*/ = is_empty_v<Compare>>
    class RedBlackCompareEmptyBaseOpt
    {
    protected:
      RedBlackCompareEmptyBaseOpt()
          : m_compare()
      {
      }
      RedBlackCompareEmptyBaseOpt(const Compare& compare)
          : m_compare(compare)
      {
      }

      Compare& get_compare()
      {
        return m_compare;
      }
      const Compare& get_compare() const
      {
        return m_compare;
      }

      template <typename T>
      bool compare(const T& lhs, const T& rhs)
      {
        return m_compare(lhs, rhs);
      }

      template <typename T>
      bool compare(const T& lhs, const T& rhs) const
      {
        return m_compare(lhs, rhs);
      }

    private:
      Compare m_compare;
    };

    template <typename Compare>
    class RedBlackCompareEmptyBaseOpt<Compare, true> : private Compare
    {
    protected:
      RedBlackCompareEmptyBaseOpt() {}
      RedBlackCompareEmptyBaseOpt(const Compare& compare)
          : Compare(compare)
      {
      }

      Compare& get_compare()
      {
        return *this;
      }
      const Compare& get_compare() const
      {
        return *this;
      }

      template <typename T, typename U = T>
      bool compare(const T& lhs, const U& rhs)
      {
        return Compare::operator()(lhs, rhs);
      }

      template <typename T, typename U = T>
      bool compare(const T& lhs, const U& rhs) const
      {
        return Compare::operator()(lhs, rhs);
      }
    };

    template <typename Key, typename Value, typename Compare, typename ExtractKey, bool bUniqueKeys, typename RBTree>
    class RedBlackTreeBase : public RedBlackCompareEmptyBaseOpt<Compare>
    {
    public:
      using extract_key = ExtractKey;

      RedBlackTreeBase() = default;
      RedBlackTreeBase(const Compare& compare)
          : RedBlackCompareEmptyBaseOpt<Compare>(compare)
      {
      }

    protected:
      using RedBlackCompareEmptyBaseOpt<Compare>::compare;
      using RedBlackCompareEmptyBaseOpt<Compare>::get_compare;
    };

    template <typename Key, typename Value, typename Compare, typename ExtractKey, typename RBTree>
    class RedBlackTreeBase<Key, Value, Compare, ExtractKey, true, RBTree> : public RedBlackCompareEmptyBaseOpt<Compare>
    {
    public:
      using extract_key = ExtractKey;

      RedBlackTreeBase() = default;
      RedBlackTreeBase(const Compare& compare)
          : RedBlackCompareEmptyBaseOpt<Compare>(compare)
      {
      }

    protected:
      using RedBlackCompareEmptyBaseOpt<Compare>::compare;
      using RedBlackCompareEmptyBaseOpt<Compare>::get_compare;
    };
    template <typename Key, typename Value, typename Compare, typename RBTree>
    class RedBlackTreeBase<Key, Value, Compare, rsl::use_first<Value>, true, RBTree> : public RedBlackCompareEmptyBaseOpt<Compare>
    {
    public:
      using extract_key = rsl::use_first<Value>;

      RedBlackTreeBase() = default;
      RedBlackTreeBase(const Compare& compare)
          : RedBlackCompareEmptyBaseOpt<Compare>(compare)
      {
      }

    protected:
      using RedBlackCompareEmptyBaseOpt<Compare>::compare;
      using RedBlackCompareEmptyBaseOpt<Compare>::get_compare;
    };
    template <typename Key, typename Value, typename Compare, typename RBTree>
    class RedBlackTreeBase<Key, Value, Compare, rsl::use_first<Value>, false, RBTree> : public RedBlackCompareEmptyBaseOpt<Compare>
    {
    public:
      using extract_key = rsl::use_first<Value>;

      RedBlackTreeBase() = default;
      RedBlackTreeBase(const Compare& compare)
          : RedBlackCompareEmptyBaseOpt<Compare>(compare)
      {
      }

    protected:
      using RedBlackCompareEmptyBaseOpt<Compare>::compare;
      using RedBlackCompareEmptyBaseOpt<Compare>::get_compare;
    };

  } // namespace v1
} // namespace rsl
