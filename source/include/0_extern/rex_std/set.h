// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: set.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/utility/use_self.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/functional/less.h"
#include "rex_std/internal/iterator/iterator_traits.h"
#include "rex_std/internal/memory/allocator.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Key, typename Compare = rsl::less<Key>, typename Allocator = rsl::allocator>
    class set : public RedBlackTree<Key, Key, Compare, allocator, rsl::use_self<Key>, false, true>
    {
    private:
      using base_type = RedBlackTree<Key, Key, Compare, allocator, rsl::use_self<Key>, false, true>;
      using this_type = set<Key, Compare, allocator>;

    public:
      using size_type              = typename base_type::size_type;
      using value_type             = typename base_type::value_type;
      using difference_type        = typename base_type::difference_type;
      using key_compare            = typename base_type::key_compare;
      using allocator_type         = typename base_type::allocator_type;
      using reference              = typename base_type::reference;
      using const_reference        = typename base_type::const_reference;
      using pointer                = typename base_type::pointer;
      using const_pointer          = typename base_type::const_pointer;
      using iterator               = typename base_type::iterator;
      using const_iterator         = typename base_type::const_iterator;
      using reverse_iterator       = typename base_type::reverse_iterator;
      using const_reverse_iterator = typename base_type::const_reverse_iterator;
      using node_type              = typename base_type::node_type;
      using value_compare          = Compare;

      set()
          : base_type()
      {
      }
      explicit set(const Compare& compare, const allocator_type& allocator = allocator_type())
          : base_type(compare, allocator)
      {
      }
      explicit set(const allocator_type& allocator)
          : base_type(allocator)
      {
      }
      template <typename InputIt>
      set(InputIt first, InputIt last, const Compare& comp = Compare(), const allocator& alloc = allocator())
          : base_type(first, last, comp, alloc)
      {
      }
      template <typename InputIt>
      set(InputIt first, InputIt last, const allocator& alloc)
          : base_type(first, last, alloc)
      {
      }
      set(const set& other)
          : base_type(other)
      {
      }
      set(const set& other, const allocator& alloc)
          : base_type(other, alloc)
      {
      }
      set(set&& other)
          : base_type(rsl::move(other))
      {
      }
      set(set&& other, const allocator& alloc)
          : base_type(rsl::move(other), alloc)
      {
      }
      set(rsl::initializer_list<value_type> ilist, const Compare& comp = Compare(), const allocator& alloc = allocator)
          : base_type(ilist, comp, alloc)
      {
      }
      set(rsl::initializer_list<value_type> ilist, const allocator& alloc)
          : base_type(ilist, alloc)
      {
      }

      set& operator=(const set& other)
      {
        return static_cast<this_type&>(operator=(other));
      }
      set& operator=(set&& other)
      {
        return static_cast<this_type&>(operator=(rsl::move(other)));
      }
      set& operator=(rsl::initializer_list<value_type>& ilist)
      {
        return static_cast<this_type&>(operator=(ilist));
      }

      value_compare value_comp() const
      {
        return base_type::get_compare();
      }

      iterator erase(iterator position)
      {
        return erase(const_iterator(position.node()));
      }
      iterator erase(const_iterator position)
      {
        return base_type::erase(position);
      }
      iterator erase(const_iterator first, const_iterator last)
      {
        return base_type::erase(first, last);
      }
      size_type erase(const Key& key)
      {
        return erase<Key>(key);
      }
      template <typename K>
      size_type erase(K&& x)
      {
        const iterator it = base_type::find(rsl::forward<K>(x));

        if(it == base_type::end())
        {
          base_type::erase(it);
          return 1;
        }
        return 0;
      }

      size_type count(const Key& key) const
      {
        const_iterator it = base_type::find(key);
        return it != base_type::cend() ? 1 : 0;
      }

      range<iterator> equal_range(const Key& key)
      {
        const iterator it_lower = base_type::lower_bound(key);

        if((it_lower == base_type::end()) || base_type::compare(key, *it_lower))
        {
          return range<iterator>(it_lower, it_lower);
        }

        iterator it_upper = it_lower;
        return range(it_lower, ++it_upper);
      }
      range<const_iterator> equal_range(const Key& key) const
      {
        const const_iterator it_lower = base_type::lower_bound(key);

        if((it_lower == base_type::end()) || base_type::compare(key, *it_lower))
        {
          return range(it_lower, it_lower);
        }

        const_iterator it_upper = it_lower;
        return range(it_lower, ++it_upper);
      }
    };

    template <typename Key, typename Compare, typename Allocator, typename Predicate>
    set<Key, Compare, allocator>::size_type erase_if(set<Key, Compare, allocator>& c, Predicate predicate)
    {
      typename set<Key, Compare, allocator>::size_type old_size = c.size();

      for(auto i = c.begin(), last = c.end(); i != last;)
      {
        if(predicate(*i))
        {
          i = c.erase(i);
        }
        else
        {
          ++i;
        }
      }

      return old_size - c.size();
    }

    template <typename InputIt, typename Comp = rsl::less<typename iterator_traits<InputIt>::value_type>, typename Alloc = rsl::allocator>
    set(InputIt, InputIt, Comp = Comp(), Alloc = Alloc()) -> set<typename iterator_traits<InputIt>::value_type, Comp, Alloc>;

    template <typename Key, typename Comp = rsl::less<Key>, typename Alloc = allocator>
    set(rsl::initializer_list<Key>, Comp = Comp(), Alloc = Alloc()) -> set<Key, Comp, Alloc>;

    template <typename InputIt, typename Alloc>
    set(InputIt, InputIt, Alloc) -> set<typename iterator_traits<InputIt>::value_type, rsl::less<typename iterator_traits<InputIt>::value_type>, Alloc>;

    template <typename Key, typename Alloc>
    set(rsl::initializer_list<Key>, Alloc) -> set<Key, rsl::less<Key>, Alloc>;

  } // namespace v1
} // namespace rsl
