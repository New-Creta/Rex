// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: map.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/binary_tree/red_black_tree.h"
#include "rex_std/bonus/iterator/iter_key.h"
#include "rex_std/bonus/iterator/iter_val.h"
#include "rex_std/bonus/utility/emplace_result.h"
#include "rex_std/bonus/utility/key_value.h"
#include "rex_std/bonus/utility/range.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/functional/less.h"
#include "rex_std/internal/iterator/iterator_traits.h"
#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/type_traits/remove_const.h"

namespace rsl
{
  inline namespace v1
  {

    //
    // map
    //

    template <typename Key, typename Value, typename Compare = rsl::less<Key>, typename Allocator = rsl::allocator>
    class map : public RedBlackTree<Key, key_value<const Key, Value>, Compare, allocator, rsl::use_first<key_value<const Key, Value>>, true, true>
    {
    private:
      using base_type = RedBlackTree<Key, key_value<Key, Value>, Compare, allocator, rsl::use_first<rsl::pair<const Key, Value>>, true, true>;
      using this_type = map<Key, Value, Compare, allocator>;

    public:
      using key_type               = typename base_type::key_type;
      using mapped_type            = Value;
      using value_type             = typename base_type::value_type;
      using size_type              = typename base_type::size_type;
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

      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Aug/2022)
      // In C++17 insert_return_type was added.
      // However, RSL doesn't support node_type extraction or insertion because it breaks encapsulation
      // so we don't define the "insert_return_type" used by these functions either.

      class value_compare
      {
      protected:
        // constructs a new value_compare object
        value_compare(Compare comp)
            : m_comp(comp)
        {
        }

      public:
        // compares lhs.first and rhs.first
        bool operator()(const value_type& lhs, const value_type& rhs)
        {
          return m_comp(lhs.key, rhs.key);
        }

      protected:
        Compare m_comp;
      };

      // constructs an empty container
      map()
          : base_type()
      {
      }
      // constructs an empty container
      explicit map(const Compare& comp, const allocator& alloc = allocator())
          : base_type(comp, allocator)
      {
      }
      // constructs an empty container
      explicit map(const allocator& alloc)
          : base_type(allocator)
      {
      }
      // constructs the container with the contents of [first, last)
      template <typename InputIt>
      map(InputIt first, InputIt last, const Compare& comp = Compare(), const allocator& alloc = allocator())
          : base_type(first, last, comp, alloc)
      {
      }
      // constructs the container with the contents of [first, last)
      template <typename InputIt>
      map(InputIt first, InputIt last, const allocator& alloc)
          : base_type(first, last, alloc)
      {
      }
      // constructs the container with the copy of the contents of other
      map(const map& other)
          : base_type(other)
      {
      }
      // constructs the container with the copy of the contents of other
      map(const map& other, const allocator& alloc)
          : base_type(other, alloc)
      {
      }
      // constructs the container with the contents of other using move semantics
      map(map&& other)
          : base_type(rsl::move(other))
      {
      }
      // constructs the container with the contents of other using move semantics
      map(map&& other, const allocator& alloc)
          : base_type(rsl::move(other), alloc)
      {
      }
      // constructs the container with the contents of the initializer list
      map(rsl::initializer_list<value_type> ilist, const Compare& comp = Compare(), const allocator& alloc = allocator())
          : base_type(ilist.begin(), ilist.end(), comp, alloc)
      {
      }
      // constructs the container with the contents of the initializer list
      map(rsl::initializer_list<value_type> ilist, const allocator& alloc)
          : base_type(ilist.begin(), ilist.end(), alloc)
      {
      }

      // destroys all elements and free heap allocated memory
      ~map() = default;

      // replaces the contents with a copy of the contents of other
      map& operator=(const map& other)
      {
        return static_cast<this_type&>(base_type::operator=(other));
      }
      // replaces the contents with those of other using move semantics
      map& operator=(map&& other)
      {
        return static_cast<this_type&>(base_type::operator=(rsl::move(other)));
      }
      // replaces the contents with those identified by the initializer list
      map& operator=(rsl::initializer_list<value_type> ilist)
      {
        return static_cast<this_type&>(base_type::operator=(ilist));
      }

      // returns a reference to the value that is mapped.
      // performs an insertion if such key does not already exist
      Value& operator[](const Key& key)
      {
        iterator it_lower = base_type::lower_bound(key);

        if((it_lower == base_type::end()) || base_type::compare(key, (*it_lower).key))
        {
          it_lower = base_type::insert(true_type(), it_lower, key);
        }

        return (*it_lower).value;
      }
      // returns a reference to the value that is mapped.
      // performs an insertion if such key does not already exist
      Value& operator[](Key&& key)
      {
        iterator it_lower = base_type::lower_bound(key);

        if((it_lower == base_type::end()) || base_type::compare(key, (*it_lower).key))
        {
          it_lower = base_type::insert(true_type(), it_lower, rsl::move(key));
        }

        return (*it_lower).value;
      }

      // returns a reference to the mapped value of the element.
      // if no such element exists, an assert is raised
      Value& at(const Key& key)
      {
        iterator it_lower = base_type::lower_bound(key);

        REX_ASSERT_X(it_lower != base_type::end(), "map key does not exist");

        return (*it_lower).value;
      }
      // returns a reference to the mapped value of the element.
      // if no such element exists, an assert is raised
      const Value& at(const Key& key)
      {
        const_iterator it_lower = base_type::lower_bound(key);

        REX_ASSERT_X(it_lower != base_type::end(), "map key does not exist");

        return (*it_lower).value;
      }

      // removes the element with the given key if one exists
      size_type erase(const Key& k)
      {
        return erase<Key>(k);
      }
      // removes the element, if one exists, that compares equivalent to the value x
      template <typename K>
      size_type erase(K&& x)
      {
        auto it = base_type::find(rsl::forward<K>(x));

        if(it != base_type::end())
        {
          base_type::erase(it);
          return 1;
        }

        return 0;
      }

      // returns the number of elements with they key that compares equivalent to the specified argument
      size_type count(const Key& key) const
      {
        const_iterator it = base_type::find(key);
        return it != base_type::cend() ? 1 : 0;
      }
      // returns the number of elements with they key that compares equivalent to the specified argument
      template <typename K>
      size_type count(const K& x) const
      {
        const_iterator it = base_type::find(x);
        return it != base_type::cend() ? 1 : 0;
      }

      range<iterator> equal_range(const Key& key)
      {
        const iterator it_lower = base_type::lower_bound(key);

        if((it_lower == base_type::end()) || base_type::compare(key, it_lower.node()->value.key))
        {
          return range(it_lower, it_lower);
        }

        iterator it_upper = it_lower;
        return range(it_lower, ++it_upper);
      }
      range<const_iterator> equal_range(const Key& key)
      {
        const const_iterator it_lower = base_type::lower_bound(key);

        if((it_lower == base_type::end()) || base_type::compare(key, it_lower.node()->value.key))
        {
          return range(it_lower, it_lower);
        }

        const_iterator it_upper = it_lower;
        return range(it_lower, ++it_upper);
      }

      // returns the comparison function object
      value_compare value_comp() const
      {
        return value_compare(base_type::get_compare());
      }
    };

    template <typename Key, typename Value, typename Compare, typename Alloc, typename Pred>
    typename map<Key, Value, Compare, Alloc>::size_type erase_if(map<Key, Value, Compare, Alloc>& c, Pred pred)
    {
      auto old_size = c.size();
      for(auto i = c.begin(), last = c.end(); i != last;)
      {
        if(pred(*i))
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

    template <typename InputIt, typename Comp = rsl::less<iter_key_t<InputIt>>, typename Alloc = rsl::allocator>
    map(InputIt, InputIt, Comp = Comp(), Alloc = allocator()) -> map<iter_key_t<InputIt>, iter_val_t<InputIt>, Comp, allocator>;

    template <typename Key, typename Value, typename Comp = rsl::less<Key>, typename Alloc = rsl::allocator>
    map(rsl::initializer_list<key_value<Key, Value>>, Comp = Comp(), allocator = allocator()) -> map<Key, Value, Comp, allocator>;

    template <typename InputIt, typename Allocator>
    map(InputIt, InputIt, allocator) -> map<iter_key_t<InputIt>, iter_val_t<InputIt>, rsl::less<iter_key_t<InputIt>>, allocator>;

    template <typename Key, typename Value, typename Allocator>
    map(rsl::initializer_list<key_value<Key, Value>>, allocator) -> map<Key, Value, rsl::less<Key>, allocator>;

    //
    // multimap
    //

    template <typename Key, typename Value, typename Compare = rsl::less<Key>, typename Allocator = rsl::allocator>
    class multimap : public RedBlackTree<Key, key_value<const Key, Value>, Compare, allocator, rsl::use_first<key_value<const Key, Value>>, true, false>
    {
    private:
      using base_type   = RedBlackTree<Key, key_value<const Key, Value>, Compare, allocator, rsl::use_first<key_value<const Key, Value>>, true, false>;
      using this_type   = multimap<Key, Value, Compare, allocator>;
      using extract_key = typename base_type::extract_key;

    public:
      using key_type               = typename base_type::key_type;
      using mapped_type            = Value;
      using value_type             = typename base_type::value_type;
      using size_type              = typename base_type::size_type;
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

      /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Aug/2022)
      // In C++17 insert_return_type was added.
      // However, RSL doesn't support node_type extraction or insertion because it breaks encapsulation
      // so we don't define the "insert_return_type" used by these functions either.

      class value_compare
      {
      protected:
        // constructs a new value_compare object
        value_compare(Compare comp)
            : m_comp(comp)
        {
        }

      public:
        // compares lhs.first and rhs.first
        bool operator()(const value_type& lhs, const value_type& rhs)
        {
          return m_comp(lhs.key, rhs.key);
        }

      protected:
        Compare m_comp;
      };

      multimap()
          : base_type()
      {
      }
      explicit multimap(const Compare& compare, const allocator_type& allocator = allocator_type())
          : base_type(compare, allocator)
      {
      }
      explicit multimap(const allocator_type& allocator)
          : base_type(allocator)
      {
      }
      template <typename InputIt>
      multimap(InputIt first, InputIt last, const Compare& comp = Compare(), const allocator_type& alloc = allocator_type())
          : base_type(first, last, comp, alloc)
      {
      }
      template <typename InputIt>
      multimap(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
          : base_type(first, last, alloc)
      {
      }
      multimap(const this_type& other)
          : base_type(other)
      {
      }
      multimap(const this_type& other, const allocator_type& alloc)
          : base_type(other, alloc)
      {
      }
      multimap(this_type&& other)
          : base_type(rsl::move(other))
      {
      }
      multimap(this_type&& other, const allocator_type& allocator)
          : base_type(rsl::move(other), allocator)
      {
      }
      multimap(rsl::initializer_list<value_type> ilist, const Compare& compare = Compare(), const allocator_type& allocator = allocator_type())
          : base_type(ilist, compare, allocator)
      {
      }
      multimap(rsl::initializer_list<value_type> ilist, const allocator_type& allocator = allocator_type())
          : base_type(ilist, allocator)
      {
      }

      multimap& operator=(const multimap& other)
      {
        return static_cast<multimap&>(base_type::operator=(other));
      }
      multimap& operator=(multimap&& other)
      {
        return static_cast<multimap&>(base_type::operator=(rsl::move(other)));
      }
      multimap& operator=(rsl::initializer_list<value_type> ilist)
      {
        return static_cast<multimap&>(base_type::operator=(ilist));
      }

      value_compare value_comp() const
      {
        return value_compare(base_type::get_compare());
      }

      size_type erase(const Key& key)
      {
        const range<iterator> range(equal_range(key));

        const size_type n = static_cast<size_type>(rsl::distance(range.first, range.last));
        base_type::erase(range.first, range.last);

        return n;
      }
      size_type count(const Key& key) const
      {
        const range<const_iterator> range = range(key);
        return rsl::distance(range.first, range.last);
      }

      range<iterator> equal_range(const Key& key)
      {
        return range<iterator>(lower_bound(key), upper_bound(key));
      }
      range<const_iterator> equal_range(const Key& key) const
      {
        return range<const_iterator>(lower_bound(key), upper_bound(key));
      }
    };

    template <typename Key, typename Value, typename Compare, typename Allocator, typename Predicate>
    typename multimap<Key, Value, Compare, allocator>::size_type erase_if(multimap<Key, Value, Compare, allocator>& c, Predicate pred)
    {
      auto old_size = c.size();

      for(auto i = c.begin(), last = c.end(); i != last;)
      {
        if(pred(*i))
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

    template <typename InputIt, typename Comp = rsl::less<iter_key_t<InputIt>>, typename Alloc = rsl::allocator>
    multimap(InputIt, InputIt, Comp = Comp(), Alloc = Alloc()) -> multimap<iter_key_t<InputIt>, iter_val_t<InputIt>, Comp, Alloc>;

    template <typename Key, typename Value, typename Comp = rsl::less<Key>, typename Alloc = rsl::allocator>
    multimap(rsl::initializer_list<key_value<Key, Value>>, Comp = Comp(), Alloc = Alloc()) -> multimap<Key, Value, Comp, Alloc>;

    template <typename InputIt, typename Alloc>
    multimap(InputIt, InputIt, Alloc) -> multimap<iter_key_t<InputIt>, iter_val_t<InputIt>, rsl::less<iter_key_t<InputIt>>, Alloc>;

    template <typename Key, typename Value, typename Allocator>
    multimap(rsl::initializer_list<key_value<Key, Value>>, allocator) -> multimap<Key, Value, rsl::less<Key>, allocator>;

  } // namespace v1
} // namespace rsl
