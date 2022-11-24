// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: node_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/hashtable/node_iterator_base.h"
#include "rex_std/bonus/type_traits/type_select.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Value, bool IsConst>
    class node_iterator : public node_iterator_base<Value>
    {
    public:
      using base_type         = node_iterator_base<Value>;
      using this_type         = node_iterator<Value, IsConst>;
      using node_type         = typename base_type::node_type;
      using value_type        = Value;
      using pointer           = typename type_select<IsConst, const Value*, Value*>::type;
      using reference         = typename type_select<IsConst, const Value&, Value&>::type;
      using difference_type   = ptrdiff_t;
      using iterator_category = forward_iterator_tag;

      explicit node_iterator(node_type* node = nullptr)
          : base_type(node)
      {
      }
      explicit node_iterator(const node_iterator<Value, true>& other)
          : base_type(other.node())
      {
      }

      reference operator*() const
      {
        return base_type::node()->value;
      }
      pointer operator->() const
      {
        return &(base_type::node()->value);
      }

      node_iterator& operator++()
      {
        base_type::increment();
        return *this;
      }
      node_iterator operator++(int)
      {
        node_iterator temp(*this);
        base_type::increment();
        return temp;
      }
    };

  } // namespace v1
} // namespace rsl