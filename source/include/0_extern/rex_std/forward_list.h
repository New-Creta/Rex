// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: forward_list.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/compare.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/limits.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      struct ForwardListNodeBase
      {
        ForwardListNodeBase* next;

        void insert_after(ForwardListNodeBase* node)
        {
          next       = node->next;
          node->next = this;
        }

        void erase_after(ForwardListNodeBase* node)
        {
          node->next = node->next->next;
        }
      };

      ForwardListNodeBase* reverse(ForwardListNodeBase* node)
      {
        ForwardListNodeBase* head = node;
        ForwardListNodeBase* next = head->next;
        head->next                = nullptr;
        while(next)
        {
          ForwardListNodeBase* tmp = next;
          next                     = head;
          head                     = node;
          node                     = tmp;
        }

        return head;
      }

      template <typename T>
      struct ForwardListNode : public ForwardListNodeBase
      {
        T value;
      };
    } // namespace internal

    // TODO: Check if we can use the ForwardIterator for this (it's implementation should change though)
    template <typename T, typename Pointer, typename Reference>
    class forward_list_iterator
    {
    private:
      using this_type = forward_list_iterator<T, Pointer, Reference>;

    public:
      using pointer   = Pointer;
      using reference = Reference;
      using node_type = internal::ForwardListNode<T>;

      forward_list_iterator()
          : m_node(nullptr)
      {
      }
      forward_list_iterator(const internal::ForwardListNodeBase* node)
          : m_node(node)
      {
      }
      forward_list_iterator(const forward_list_iterator& other)
          : m_node(other.m_node)
      {
      }

      reference operator*() const
      {
        return m_node->value;
      }
      pointer operator->() const
      {
        return &m_node->value;
      }

      this_type& operator++()
      {
        m_node = static_cast<node_type*>(m_node->next);
        return *this;
      }
      this_type operator++(int)
      {
        this_type tmp(*this);
        m_node = static_cast<node_type*>(m_node->next);
        return tmp;
      }

      const node_type* node() const
      {
        return m_node;
      }

    private:
      node_type* m_node;
    };

    template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
    constexpr bool operator==(const forward_list_iterator<T, PointerA, ReferenceA>& lhs, const forward_list_iterator<T, PointerB, ReferenceB>& rhs)
    {
      return lhs.node() == rhs.node();
    }
    template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
    constexpr bool operator!=(const forward_list_iterator<T, PointerA, ReferenceA>& lhs, const forward_list_iterator<T, PointerB, ReferenceB>& rhs)
    {
      return lhs.node() != rhs.node();
    }

    /// According to the C++ standard at time of writing (08/June/2022)
    /// the single linked list does not have a size function, while the double linked list does.
    /// This is breaking consistency between these 2 containers who are believed to be pretty similar.
    /// Because of this and because having a cached size only improves performance when checking the size,
    /// while insertion and erasing, which happens more often, would become slower, we don't have a size function
    /// for both the single list and the double linked list.

    template <typename T, typename Alloc = rsl::allocator>
    class forward_list
    {
    public:
      using value_type      = T;
      using allocator_type  = Alloc;
      using size_type       = count_t; /// RSL Comment: Different From ISO C++ Standard at time of writing (04/June/2022)
      using difference_type = int32;
      using reference       = value_type&;
      using const_reference = const value_type&;
      using pointer         = typename rsl::allocator_traits<allocator>::pointer;
      using const_pointer   = typename rsl::allocator_traits<allocator>::const_pointer;
      using iterator        = rsl::forward_list_iterator<value_type, pointer, reference>;
      using const_iterator  = rsl::forward_list_iterator<value_type, const_pointer, const_reference>;
      using node_type       = rsl::internal::ForwardListNode<T>;
      using base_node_type  = rsl::internal::ForwardListNodeBase;
      using this_type       = forward_list<T, Alloc>; /// RSL Comment: Not in ISO C++ Standard at time of writing (04/June/2022)

      forward_list()
          : m_cp_pre_head_node_and_allocator()
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
      }
      explicit forward_list(const allocator_type& alloc)
          : m_cp_pre_head_node_and_allocator(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
      }
      forward_list(size_type count, const_reference value, const allocator_type& alloc = allocator_type())
          : m_cp_pre_head_node_and_allocator(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(count)
#endif
      {
        insert_impl(end(), count, value);
      }
      explicit forward_list(size_type count, const allocator_type& alloc = allocator_type())
          : m_cp_pre_head_node_and_allocator(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(count)
#endif
      {
        insert_impl(end(), count);
      }
      template <typename InputIt>
      forward_list(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
          : m_cp_pre_head_node_and_allocator(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        insert_impl(first, last);
      }
      forward_list(const forward_list& other)
          : m_cp_pre_head_node_and_allocator(other.get_allocator())
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(other.m_size)
#endif
      {
        insert_impl(other.cbegin(), other.cend());
      }
      forward_list(const forward_list& other, const allocator_type& alloc)
          : m_cp_pre_head_node_and_allocator(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(other.m_size)
#endif
      {
        insert_impl(other.cbegin(), other.cend());
      }
      forward_list(forward_list&& other)
          : m_cp_pre_head_node_and_allocator(rsl::move(other.get_allocator()))
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        swap(other);
      }
      forward_list(forward_list&& other, const allocator_type& alloc)
          : m_cp_pre_head_node_and_allocator(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        swap(other);
      }
      forward_list(rsl::initializer_list<value_type> ilist, const allocator_type& alloc)
          : m_cp_pre_head_node_and_allocator(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        insert_impl(ilist.begin(), ilist.end());
      }

      ~forward_list()
      {
        clear();
      }

      forward_list& operator=(const forward_list& other)
      {
        assign(other.cbegin(), other.cend());
        return *this;
      }
      forward_list& operator=(forward_list&& other)
      {
        swap(other);
        return *this;
      }
      forward_list& operator=(rsl::initializer_list<value_type> ilist)
      {
        assign(ilist.begin(), ilist.end());
      }

      void assign(size_type count, const T& value)
      {
        internal::ForwardListNodeBase* prev_node = &pre_head_node();
        node_type* node                          = head();
        while(node && count > 0)
        {
          node->value = value;
          prev_node   = node;
          node        = static_cast<node_type*>(node->next);
          --count;
        }

        if(count > 0)
        {
          emplace_after_impl(prev_node, count, value);
        }
        else
        {
          erase_after_impl(prev_node, end());
        }
      }
      template <typename InputIterator>
      void assign(InputIterator first, InputIterator last)
      {
        internal::ForwardListNodeBase* prev_node = &pre_head_node();
        node_type* node                          = head();
        while(node && first != last)
        {
          node->value = *first;
          node        = static_cast<node_type*>(node->next);
          ++first;
        }

        // m_size hasn't changed yet, we expect insert and erase to take care of that.

        if(first != last)
        {
          emplace_after_impl(prev_node, first, last);
        }
        else
        {
          erase_after_impl(prev_node, end());
        }
      }
      void assign(rsl::initializer_list<T> ilist)
      {
        assign(ilist.begin(), ilist.end());
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (19/Jun/2022)
      // This returns a copy in the standard.
      const allocator_type& get_allocator() const
      {
        return m_cp_pre_head_node_and_allocator.second();
      }

      // element access
      T& front()
      {
        return head()->value;
      }
      const T& front() const
      {
        return head()->value;
      }

      iterator before_begin()
      {
        return iterator(pre_head_node());
      }
      const_iterator cbefore_begin() const
      {
        return const_iterator(pre_head_node());
      }
      iterator begin()
      {
        return iterator(head());
      }
      const_iterator begin() const
      {
        return const_iterator(head());
      }
      const_iterator cbegin()
      {
        return const_iterator(head());
      }

      /// According to the C++ ISO Standard, this acts as a place holder
      /// attempting to access it resolves in undefined behavior.
      /// So this can be pretty much anything we want.
      /// But as we do want to crash or get some kind of notification
      /// when we're trying to access it, we'll initialize it with nullptr.
      /// We could initialize it with the pre_head_node as well, but when we're dereferencing that
      /// That'll result in undefined behavior that isn't guarnateed to crash, while accessing a nullptr is.
      iterator end()
      {
        return iterator(nullptr);
      }
      const_iterator end() const
      {
        return const_iterator(nullptr);
      }
      const_iterator cend() const
      {
        return const_iterator(nullptr);
      }

      // capacity
      bool empty() const
      {
        head() == nullptr;
      }
      size_type max_size()
      {
        return numeric_limits<difference_type>::max();
      }

      // modifiers
      void clear()
      {
        node_type* node = head();
        while(node)
        {
          node_type* tmp_node = node;
          node                = node->next;
          get_allocator().deallocate(tmp_node);
        }

#ifdef REX_ENABLE_SIZE_IN_LISTS
        m_size = 0;
#endif
      }
      void insert_after(const_iterator pos, const_reference value)
      {
        emplace_after(pos, value);
      }
      void insert_after(const_iterator pos, value_type&& value)
      {
        emplace_after(pos, rsl::move(value));
      }
      void insert_after(const_iterator pos, size_type count, const_reference value)
      {
        while(count > 0)
        {
          emplace_after_impl(pos.m_node, value);
        }
      }
      template <typename InputIt>
      void insert_after(const_iterator pos, InputIt first, InputIt last)
      {
        while(first != last)
        {
          emplace_after_impl(pos.m_node, *first);
          ++first;
        }
      }
      void insert_after(const_iterator pos, rsl::initializer_list<T> ilist)
      {
        insert_after(pos, ilist.begin(), ilist.end());
      }
      template <typename... Args>
      void emplace_after(const_iterator pos, Args&&... args)
      {
        emplace_after_impl(pos.m_node, args);
      }
      iterator erase_after(const_iterator pos)
      {
        node_type* next_node = pos.m_node->next;
        pos.m_node->erase_after();

        get_allocator().destroy(next_node);
        get_allocator().deallocate(next_node);

#ifdef REX_ENABLE_SIZE_IN_LIST
        --m_size;
#endif
      }
      iterator erase_after(const_iterator first, const_iterator last)
      {
        while(first != last)
        {
          auto current_node = first++;
          erase_after(current_node);
        }
      }
      void push_front(const value_type& value)
      {
        emplace_front(value);
      }
      void push_front(value_type&& value)
      {
        emplace_front(rsl::move(value));
      }
      template <typename... Args>
      void emplace_front(Args&&... args)
      {
        emplace_after(before_begin(), rsl::forward<Args>(args)...);
      }
      void pop_front()
      {
        erase_after(before_begin());
      }
      void resize(size_type count)
      {
        node_type* node = find_nth(count);

        if(node->next)
        {
          erase_after(const_iterator(node), cend());
        }
        else
        {
          emplace_after(const_iterator(node));
        }
      }
      void resize(size_type count, const_reference value)
      {
        node_type* node = find_nth(count);

        if(node->next)
        {
          erase_after(const_iterator(node), cend());
        }
        else
        {
          emplace_after(const_iterator(node), value);
        }
      }
      void swap(this_type& other)
      {
        if(get_allocator() == other.get_allocator())
        {
          rsl::swap(head(), other.head());
          rsl::swap(get_allocator(), other.get_allocator());

#ifdef REX_ENABLE_SIZE_IN_LISTS
          rsl::swap(m_size, other.m_size);
#endif
        }
        else
        {
          const this_type tmp(*this);
          *this = other;
          other = tmp;
        }
      }

      // operations
      // TODO: implement operations
      void merge() {}
      void splice_after() {}
      void remove() {}
      void remove_if() {}
      void reverse()
      {
        if(head())
        {
          pre_head_node().next = static_cast<node_type*>(rsl::internal::reverse(static_cast<base_node_type*>(head())));
        }
      }
      void unique() {}
      void sort() {}

    private:
      template <typename... Args>
      void emplace_after_impl(node_type* node, Args&&... args)
      {
        node_type* new_node = get_allocator().allocate(1); // TODO: use 1_elem here instead of literal 1 to make it clear we're using element count instead of number of bytes
        get_allocator().construct(new_node, rsl::forward<Args>(args)...);
        new_node->insert_after(node);

#ifdef REX_ENABLE_SIZE_IN_LISTS
        ++m_size;
#endif
      }

      node_type* head()
      {
        pre_head_node().next;
      }

      // Find's the nth node or end if n > size
      node_type* find_nth(size_type n)
      {
        node_type* node = head();

        while(node && n > 0)
        {
          --n;
          node = node->next;
        }

        return node;
      }

      internal::ForwardListNodeBase& pre_head_node()
      {
        return m_cp_pre_head_node_and_allocator.first();
      }

      allocator_type& get_allocator()
      {
        return m_cp_pre_head_node_and_allocator.second();
      }

    private:
      rsl::compressed_pair<internal::ForwardListNodeBase, allocator_type> m_cp_pre_head_node_and_allocator;

#ifdef REX_ENABLE_SIZE_IN_LISTS
      size_type m_size;
#endif
    };

  } // namespace v1
} // namespace rsl
