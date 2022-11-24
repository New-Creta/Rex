// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: list.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/memory/uninitialized_default_construct.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/functional/less.h"
#include "rex_std/internal/iterator/distance.h"
#include "rex_std/internal/iterator/reverse_iterator.h"
#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/memory/allocator_traits.h"
#include "rex_std/internal/memory/destroy_at.h"
#include "rex_std/internal/utility/exchange.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/move.h"
#include "rex_std/internal/utility/swap.h"
#include "rex_std/iterator.h"
#include "rex_std/limits.h"

namespace rsl
{
  inline namespace v1
  {

    struct ListNodeBase
    {
      ListNodeBase* next;
      ListNodeBase* prev;

      void insert(ListNodeBase* node)
      {
        next       = node;
        prev       = node->prev;
        prev->next = this;
        node->prev = this;
      }

      void remove()
      {
        next->prev = prev;
        prev->next = next;
      }

      void splice(ListNodeBase* first, ListNodeBase* last)
      {
        last->prev->next  = this;
        first->prev->next = last;
        prev->next        = first;

        ListNodeBase* tmp = prev;
        prev              = last->prev;
        last->prev        = first->prev;
        first->prev       = tmp;
      }

      void reverse()
      {
        ListNodeBase* node = this;
        do
        {
          ListNodeBase* tmp = node->next;
          node->next        = node->prev;
          node->prev        = tmp;
          node              = node->prev;
        } while(node != this);
      }
    };

    template <typename T>
    struct ListNode : public ListNodeBase
    {
      T value;
    };

    template <typename T, typename Allocator>
    class list;

    // TODO: Check if we can use the BiDirectional Iterator for this (it's implementation should change though)
    template <typename T>
    class ListIterator
    {
    public:
      using this_type         = ListIterator<T>;
      using node_type         = ListNode<T>;
      using value_type        = T;
      using pointer           = T*;
      using const_pointer     = const T*;
      using reference         = T&;
      using const_reference   = const T&;
      using iterator_category = rsl::bidirectional_iterator_tag;
      using difference_type   = int32;

      template <typename T, typename Allocator>
      friend class list;

      reference operator*()
      {
        return m_node->value;
      }
      const_reference operator*() const
      {
        return m_node->value;
      }
      pointer operator->()
      {
        return rsl::addressof(m_node->value);
      }
      const_pointer operator->() const
      {
        return rsl::addressof(m_node->value);
      }

      ListIterator<T> operator++()
      {
        m_node = static_cast<node_type*>(m_node->next);
        return *this;
      }
      ListIterator<T> operator++(int)
      {
        this_type temp(*this);
        m_node = static_cast<node_type*>(m_node->next);
        return temp;
      }
      ListIterator<T> operator--()
      {
        m_node = static_cast<node_type*>(m_node->prev);
        return *this;
      }
      ListIterator operator--(int)
      {
        this_type temp(*this);
        m_node = static_cast<node_type*>(m_node->prev);
        return temp;
      }

    private:
      node_type* m_node;
    };

    template <typename T>
    class ConstListIterator
    {
    public:
      using this_type         = ListIterator<T>;
      using node_type         = ListNode<T>;
      using value_type        = T;
      using pointer           = T*;
      using const_pointer     = const T*;
      using reference         = T&;
      using const_reference   = const T&;
      using iterator_category = rsl::bidirectional_iterator_tag;
      using difference_type   = int32;

      template <typename T, typename Allocator>
      friend class list;

      const_reference operator*() const
      {
        return m_node->value;
      }
      const_pointer operator->() const
      {
        return rsl::addressof(m_node->value);
      }

      ConstListIterator<T> operator++()
      {
        m_node = static_cast<node_type*>(m_node->next);
        return *this;
      }
      ConstListIterator<T> operator++(int)
      {
        this_type temp(*this);
        m_node = static_cast<node_type*>(m_node->next);
        return temp;
      }
      ConstListIterator<T> operator--()
      {
        m_node = static_cast<node_type*>(m_node->prev);
        return *this;
      }
      ConstListIterator operator--(int)
      {
        this_type temp(*this);
        m_node = static_cast<node_type*>(m_node->prev);
        return temp;
      }

    private:
      node_type* m_node;
    };

    /// Some libraries (eg. EASTL and GCC) use a base class for some of its containers because it makes it easier to handle exceptions
    /// as the base classes of the containers handle memory allocation.
    /// so if a container is created which needs to allocate memory in the constructor, the base class handles it.
    /// If an exception is thrown by the allocator, then the exception throw jumps back to the user code
    /// no try/catch code needs to be written in the container or the container base constructor.
    /// if an exception is thrown in the container (not the container base) constructor, the destructor of container base
    /// will automatically be called before the execution jumps back to the user code.
    /// This is to avoid implementing try/catch blocks in every pathway in the container class as this makes hurts performance and increases code size.
    /// RSL disables exceptions in the entire codebase and we use asserts instead, for this reason, we don't implement base classes for our containers.

    template <typename T, typename Alloc = allocator>
    class list
    {
    public:
      using value_type             = T;
      using allocator_type         = Alloc;
      using size_type              = count_t; /// RSL Comment: Different From ISO C++ Standard at time of writing (19/May/2022)
      using difference_type        = int32;
      using reference              = value_type&;
      using const_reference        = const value_type&;
      using pointer                = typename allocator_traits<allocator>::pointer;
      using const_pointer          = typename allocator_traits<allocator>::const_pointer;
      using iterator               = ListIterator<value_type>;
      using const_iterator         = ListIterator<const value_type>;
      using reverse_iterator       = rsl::reverse_iterator<iterator>;
      using const_reverse_iterator = rsl::reverse_iterator<const_iterator>;
      using node_type              = ListNode<T>;    /// RSL Comment: Not in ISO C++ Standard at time of writing (04/June/2022)
      using this_type              = list<T, Alloc>; /// RSL Comment: Not in ISO C++ Standard at time of writing (04/June/2022)

      list()
          : m_cp_head_tail_link_and_alloc()
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        init();
      }
      explicit list(const allocator_type& alloc)
          : m_cp_head_tail_link_and_alloc(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        init();
      }
      list(size_type count, const_reference value, const allocator_type& alloc = allocator_type())
          : m_cp_head_tail_link_and_alloc(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(count)
#endif
      {
        insert(end(), count, value);
      }
      explicit list(size_type count, const allocator_type& alloc = allocator_type())
          : m_cp_head_tail_link_and_alloc(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(count)
#endif
      {
        insert(end(), count);
      }
      template <typename InputIt>
      list(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
          : m_cp_head_tail_link_and_alloc(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        insert(end(), first, last);
      }
      list(const list& other)
          : m_cp_head_tail_link_and_alloc(other.get_allocator())
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(other.m_size)
#endif
      {
        insert(end(), other.cbegin(), other.cend());
      }
      list(const list& other, const allocator_type& alloc)
          : m_cp_head_tail_link_and_alloc(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(other.m_size)
#endif
      {
        insert(end(), other.cbegin(), other.cend());
      }
      list(list&& other)
          : m_cp_head_tail_link_and_alloc(rsl::move(other.get_allocator()))
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        swap(other);
      }
      list(list&& other, const allocator_type& alloc)
          : m_cp_head_tail_link_and_alloc(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        swap(other);
      }
      list(rsl::initializer_list<value_type> ilist, const allocator_type& alloc)
          : m_cp_head_tail_link_and_alloc(alloc)
#ifdef REX_ENABLE_SIZE_IN_LISTS
          , m_size(0)
#endif
      {
        insert(end(), ilist.begin(), ilist.end());
      }

      ~list()
      {
        clear();
      }

      list& operator=(const list& other)
      {
        assign(other.cbegin(), other.cend());

        return *this;
      }
      list& operator=(list&& other) noexcept
      {
        REX_ASSERT_X(this != &other, "Can't move assign to yourself!");
        swap(other);

        return *this;
      }
      list& operator=(rsl::initializer_list<value_type> ilist)
      {
        assign(ilist.begin(), ilist.end());

        return *this;
      }

      void assign(size_type count, const T& value)
      {
        node_type* node = head();
        while(node != head_tail_link() && count > 0)
        {
          node->value = value;
          node        = static_cast<node_type>(node->next);
          --count;
        }

        // m_size hasn't changed yet, we expect insert and erase to take care of that.

        if(count > 0)
        {
          insert(end(), count, value);
        }
        else
        {
          erase(const_iterator(node), end());
        }
      }
      template <typename InputIterator>
      void assign(InputIterator first, InputIterator last)
      {
        node_type* node = head();
        while(node != head_tail_link() && first != last)
        {
          node->value = *first;
          node        = static_cast<node_type>(node->next);
          ++first;
        }

        // m_size hasn't changed yet, we expect insert and erase to take care of that.

        if(first != last)
        {
          insert(end(), first, last);
        }
        else
        {
          erase(const_iterator(node), end());
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
        return m_cp_head_tail_link_and_alloc.second();
      }

      // element access
      reference front()
      {
        return head()->value;
      }
      const_reference front() const
      {
        return head()->value;
      }
      reference back()
      {
        return tail()->value;
      }
      const_reference back() const
      {
        return tail()->value;
      }

      // iterators
      iterator begin()
      {
        return iterator(head());
      }
      const_iterator begin() const
      {
        return const_iterator(head());
      }
      const_iterator cbegin() const
      {
        return const_iterator(head());
      }

      iterator end()
      {
        return iterator(head_tail_link());
      }
      const_iterator end() const
      {
        return const_iterator(head_tail_link());
      }
      const_iterator cend() const
      {
        return const_iterator(head_tail_link());
      }

      reverse_iterator rbegin()
      {
        return reverse_iterator(end());
      }
      const_reverse_iterator rbegin() const
      {
        return const_reverse_iterator(end());
      }
      const_reverse_iterator crbegin() const
      {
        return const_reverse_iterator(cend());
      }

      reverse_iterator rend()
      {
        return reverse_iterator(begin());
      }
      const_reverse_iterator rend() const
      {
        return const_reverse_iterator(begin());
      }
      const_reverse_iterator crend() const
      {
        return const_reverse_iterator(cbegin());
      }

      // capacity
      bool empty() const
      {
        size() != 0;
      }
      size_type size() const
      {
#ifdef REX_ENABLE_SIZE_IN_LISTS
        return m_size;
#else
        size_type count = 0;

        for(auto first = cbegin(), last = cend(); first != last; ++first)
        {
          ++count;
        }

        return count;
#endif
      }
      size_type max_size() const
      {
        return numeric_limits<difference_type>::max();
      }

      // modifiers
      void clear()
      {
        node_type* node = head();
        while(node != head_tail_link())
        {
          node_type* tmp_node = node;
          node                = static_cast<node_type*>(node->next);
          get_allocator().destroy(tmp_node);
          get_allocator().deallocate(tmp_node);
        }

#ifdef REX_ENABLE_SIZE_IN_LISTS
        m_size = 0;
#endif
      }
      iterator insert(const_iterator pos, const T& value)
      {
        return emplace(pos, value);
      }
      iterator insert(const_iterator pos, T&& value)
      {
        return emplace(pos, rsl::move(value));
      }
      iterator insert(const_iterator pos, size_type count, const T& value)
      {
        // break const here, because we're getting the non-const node of a const_iterator
        node_type* pos_node  = pos.m_node;
        node_type* prev_node = pos_node->prev;

        // We can't allocate all nodes in 1 go as this would allocate a buffer
        // which would have to be deallocated using the address of the buffer start
        // So we have to allocate nodes, 1 at a time.
        // With the proper allocator, we might be able to fix this so the allocater itself
        // predicts it will allocate multiple elements and therefore will allocate a buffer
        // that can be 'split'.
        while(count > 0)
        {
          node_type* new_node = static_cast<node_type*>(get_allocator().allocate(1)); // TODO: use 1_elem here instead of literal 1 to make it clear we're using element count instead of number of bytes
          insert_at(pos_node, value, new_node);
          --count;
#ifdef REX_ENABLE_SIZE_IN_LISTS
          ++m_size;
#endif
        }

        return iterator(prev_node->next);
      }
      template <typename InputIterator>
      iterator insert(const_iterator pos, InputIterator first, InputIterator last)
      {
        // break const here, because we're getting the non-const node of a const_iterator
        node_type* pos_node  = pos.m_node;
        node_type* prev_node = pos_node->prev;

        // We can't allocate all nodes in 1 go as this would allocate a buffer
        // which would have to be deallocated using the address of the buffer start
        // So we have to allocate nodes, 1 at a time.
        // With the proper allocator, we might be able to fix this so the allocater itself
        // predicts it will allocate multiple elements and therefore will allocate a buffer
        // that can be 'split'.
        while(first != last)
        {
          node_type* new_node = static_cast<node_type*>(get_allocator().allocate(1)); // TODO: use 1_elem here instead of literal 1 to make it clear we're using element count instead of number of bytes
          insert_at(pos, *first, new_node);
          ++first;
#ifdef REX_ENABLE_SIZE_IN_LISTS
          ++m_size;
#endif
        }

        return prev_node->next;
      }
      iterator insert(const_iterator pos, rsl::initializer_list<T> ilist)
      {
        return insert(pos, ilist.begin(), ilist.end());
      }
      template <typename... Args>
      iterator emplace(const_iterator pos, Args&&... args)
      {
        node_type* new_node = static_cast<node_type*>(get_allocator().allocate(1)); // TODO: use 1_elem here instead of literal 1 to make it clear we're using element count instead of number of bytes
        new(rsl::addressof(new_node->value)) value_type(rsl::forward<Args>(args)...);
        new_node->insert(pos);
        return iterator(new_node);
      }
      iterator erase(const_iterator pos)
      {
        // break const here, because we're getting the non-const node of a const_iterator
        node_type* pos_node = pos.m_node;

        pos_node->remove();
        get_allocator().destroy(pos_node);
        get_allocator().deallocate(pos_node, sizeof(node_type));

#ifdef REX_ENABLE_SIZE_IN_LISTS
        --m_size;
#endif
        ++pos;
        return iterator(pos.m_node);
      }
      iterator erase(const_iterator first, const_iterator last)
      {
        while(first != last)
        {
          erase(const_iterator(first));
          ++first;
        }

        return iterator(last.m_node);
      }
      void push_back(const value_type& value)
      {
        emplace_back(value);
      }
      void push_back(value_type&& value)
      {
        emplace_back(rsl::move(value));
      }
      template <typename... Args>
      void emplace_back(Args&&... args)
      {
        node_type* new_node = static_cast<node_type*>(get_allocator().allocate(1)); // TODO: use 1_elem here instead of literal 1 to make it clear we're using element count instead of number of bytes
        new(rsl::addressof(new_node->value)) value_type(rsl::forward<Args>(args)...);
        head_tail_link()->insert(new_node);
      }
      void pop_back()
      {
        erase(const_iterator(tail()));
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
        emplace(const_iterator(head()), rsl::forward<Args>(args)...);
      }
      void pop_front()
      {
        erase(head());
      }
      void resize(size_type new_size)
      {
        size_type num_new_elements = new_size - size();
#ifdef REX_ENABLE_SIZE_IN_LISTS
        m_size = new_size;
#endif
        while(num_new_elements > 0)
        {
          node_type* new_node = static_cast<node_type*>(get_allocator().allocate(1)); // TODO: use 1_elem here instead of literal 1 to make it clear we're using element count instead of number of bytes
          rsl::uninitialized_default_construct(new_node);                             // kind of annoying we can't do this in the allocator
          new_node->insert(head_tail_link());
          --num_new_elements;
        }
      }
      void resize(size_type new_size, const value_type& value)
      {
        size_type num_new_elements = new_size - size();
#ifdef REX_ENABLE_SIZE_IN_LISTS
        m_size = new_size;
#endif
        while(num_new_elements > 0)
        {
          node_type* new_node = static_cast<node_type*>(get_allocator().allocate(1)); // TODO: use 1_elem here instead of literal 1 to make it clear we're using element count instead of number of bytes
          get_allocator().construct(new_node, value);
          new_node->insert(head_tail_link());
          --num_new_elements;
        }
      }
      void swap(list<T, allocator_type>& other)
      {
        rsl::swap(m_cp_head_tail_link_and_alloc, other.m_cp_head_tail_link_and_alloc);
        rsl::swap(get_allocator(), other.get_allocator());
      }

      // operations
      void merge(this_type& other)
      {
        merge(other, rsl::less<T>());
      }
      void merge(this_type&& other)
      {
        merge(other, rsl::less<T>()); // this will call merge as if 'other' is a lvalue ref
      }
      template <typename Compare>
      void merge(this_type& other, Compare compare)
      {
        if(this == &other)
        {
          return;
        }

        iterator first            = begin();
        iterator other_first      = other.begin();
        const iterator last       = end();
        const iterator other_last = other.end();

        while((first != last) && (other_first != other_last))
        {
          if(compare(*other_first, *first))
          {
            iterator next(other_first);
            splice(first, other, other_first, ++next);
            other_first = next;
          }
          else
          {
            ++first;
          }
        }

        if(other_first != other_last)
        {
          splice(last, other, other_first, other_last);
        }
      }
      template <typename Compare>
      void merge(this_type&& other, Compare compare)
      {
        return merge(other, compare); // this will call merge as if 'other' is a lvalue ref
      }

      void splice(const_iterator pos, this_type& other)
      {
        if(get_allocator() == other.get_allocator())
        {
          if(!other.empty())
          {
            // Breaking const here, getting non-const node from const iterator
            ListNodeBase* pos_base   = static_cast<ListNodeBase*>(pos.m_node);
            ListNodeBase* begin_base = static_cast<ListNodeBase*>(other.cbegin().m_node);
            ListNodeBase* end_base   = static_cast<ListNodeBase*>(other.cend().m_node);
            pos_base->splice(begin_base, end_base);
          }
        }
      }
      void splice(const_iterator pos, this_type&& other)
      {
        return splice(pos, other); // this will call splice as if 'other' is a lvalue ref
      }
      void splice(const_iterator pos, this_type& other, const_iterator it)
      {
        if(get_allocator() == other.get_allocator())
        {
          const_iterator it2(it.m_node);
          ++it2;
          if((pos != it) && (pos != it2))
          {
            // Breaking const here, getting non-const node from const iterator
            ListNodeBase* pos_base = static_cast<ListNodeBase*>(pos.m_node);
            ListNodeBase* it_base  = static_cast<ListNodeBase*>(it.m_node);
            ListNodeBase* it2_base = static_cast<ListNodeBase*>(it2.m_node);
            pos_base->splice(it_base, it2_base);
          }
        }
      }
      void splice(const_iterator pos, this_type&& other, const_iterator it)
      {
        splice(pos, other, it); // this will call splice as if 'other' is a lvalue ref
      }
      void splice(const_iterator pos, this_type& other, const_iterator first, const_iterator last)
      {
        if(get_allocator() == other.get_allocator())
        {
          if(first != last)
          {
            // Breaking const here, getting non-const node from const iterator
            ListNodeBase* pos_base   = static_cast<ListNodeBase*>(pos.m_node);
            ListNodeBase* first_base = static_cast<ListNodeBase*>(first.m_node);
            ListNodeBase* last_base  = static_cast<ListNodeBase*>(last.m_node);
            pos_base->splice(first_base, last_base);
          }
        }
        else
        {
          insert(pos, first, last);
          other.erase(first, last);
        }
      }
      void splice(const_iterator pos, this_type&& other, const_iterator first, const_iterator last)
      {
        return splice(pos, other, first, last); // this will call splice as if 'other' is a lvalue ref
      }

      size_type remove(const value_type& value)
      {
        return remove_if([](const_reference& lhs, const_reference rhs) { return lhs == rhs; });
      }
      template <typename Predicate>
      size_type remove_if(Predicate pred)
      {
        size_type num_removed = 0;
        iterator it           = begin();

        while(it.m_node != head_tail_link())
        {
          if(!(pred(*it)))
          {
            ++it;
          }
          else
          {
            ++it;
            ++num_removed;
            it.m_node->remove();
            get_allocator().destroy(it.m_node);
            get_allocator().deallocate(it.m_node);
          }
        }

        return num_removed;
      }
      void reverse()
      {
        static_cast<ListNodeBase*>(head_tail_link())->reverse();
      }
      size_type unique()
      {
        unique([](const_reference lhs, const_reference rhs) { return lhs == rhs; });
      }
      template <typename Predicate>
      size_type unique(Predicate pred)
      {
        iterator it         = begin();
        const iterator last = end();

        size_type num_removed = 0;

        if(it != last)
        {
          iterator next(it);

          while(++next != last)
          {
            if(pred(*it, *next))
            {
              ++num_removed;
              it.m_node->remove();
              get_allocator().destroy(it.m_node);
              get_allocator().deallocate(it.m_node);
            }
            else
            {
              it = next;
            }
            next = it;
          }
        }

        return num_removed;
      }
      void sort()
      {
        sort_impl(begin(), end(), size(), rsl::less);
      }
      template <typename Compare>
      void sort(const Compare& compare)
      {
        sort_impl(begin(), end(), size(), compare);
      }

    private:
      void init()
      {
        head_tail_link().next = head_tail_link();
        head_tail_link().prev = head_tail_link();
      }

      node_type* head()
      {
        static_cast<ListNode*>(head_tail_link().next);
      }
      node_type* tail()
      {
        static_cast<ListNode*>(head_tail_link().prev);
      }
      ListNodeBase* head_tail_link()
      {
        return m_cp_head_tail_link_and_alloc.first();
      }

      void insert_at(node_type* pos, const T& value, node_type* new_node)
      {
        new_node->value = value;
        new_node->insert(pos);
      }

      template <typename Compare>
      void sort_impl(iterator first, iterator last, size_type size, Compare& compare)
      {
        switch(size)
        {
          case 0:
          case 1: return first;
          case 2:
            if(compare(*--last, *first))
            {
              last.m_node->remove();
              last.m_node->insert(first.m_node);
            }
            return first;
          case 3:
          {
            iterator lowest = first;

            for(iterator current = first.next(); current != last; ++current)
            {
              if(compare(*current, *lowest))
              {
                lowest = current;
              }
            }

            if(lowest == first)
            {
              ++first;
            }
            else
            {
              lowest.m_node->remove();
              lowest.m_node->insert(first.m_node);
            }

            if(compare(*--last, *first))
            {
              last.m_node->remove();
              last.m_node->insert(first.m_node);
            }

            return lowest;
          }
        }

        // divide the range into 2 parts and recrusively sort each part. Upon rturn we will have
        // two halves tha are each sorted but we'll need to merge the two together before returning
        iterator result;
        size_type half_size  = size / 2;
        iterator mid_end     = rsl::next(first, static_cast<difference_type>(half_size));
        iterator first_half  = sort_impl(first, mid_end, half_size, compare);
        iterator second_half = sort_impl(mid_end, last, size - half_size, compare);

        // If the start of the second list is before the start of the first list, insert the first list
        // into the second at an appropriate starting place
        if(compare(*second_half, *first_half))
        {
          // find the position to insert eh first list into the second list
          iterator ix = second_half.next();
          while((ix != last) && (*ix, *first))
          {
            ++ix;
          }

          // cut out the initial segment of the second list and move it to be in front of the first list
          ListNodeBase* second_half_cut      = second_half.m_node;
          ListNodeBase* second_half_cut_last = ix.m_node->prev;
          result                             = second_half;
          mid_end = second_half = ix;
          ListNodeBase::remove_range(second_half_cut, second_half_cut_last);
          first.m_node->insert_range(second_half_cut, second_half_cut_last);
        }
        else
        {
          result  = first;
          mid_end = second_half;
        }

        // merge the two segements. we do this by merging the second sub-segment into the first, by walking forward in each othe the two sub-segments.
        for(++first; (first != mid_end) && (second_half != last); ++first)
        {
          if(compare(*second_half, *first))
          {
            iterator ix = second_half.next();
            while((ix != last) && compare(*ix, *first))
            {
              ++ix;
            }

            // Cut this section of the second_half sub segment out and merge into the appropriate place in the first_half list.
            ListNodeBase* second_half_cut      = second_half.m_node;
            ListNodeBase* second_half_cut_last = ix.m_node->prev;
            if(mid_end == second_half)
            {
              mid_end = ix;
            }
            second_half = ix;
            ListNodeBase::remove_range(second_half_cut, second_half_cut_last);
            first.m_node->insert_range(second_half_cut, second_half_cut_last);
          }
        }

        return result;
      }

      allocator_type& get_allocator()
      {
        return m_cp_head_tail_link_and_alloc.second();
      }

    private:
      /// head_tail_link() is a fictional node that sits between the last and the first node.
      /// it is mainly used to create the begin and end iterators
      ///
      ///     +---------------------------------------------------------------------------------+
      ///     |                                                                                 |
      ///     v                                                                                 |
      /// +--------+      +--------+      +--------+      +--------+      +--------+      + - - - - - +
      /// |        |      |        |      |        |      |        |      |        |      |           |
      /// |        | <--> |        | <--> |        | <--> |        | <--> |        | <--> | head tail |
      /// |        |      |        |      |        |      |        |      |        |      |   link    |
      /// +--------+      +--------+      +--------+      +--------+      +--------+      + - - - - - +
      ///

      /// head_tail_link().next points to the first node in the list
      /// head_tail_link().prev points to the last node in the list

      compressed_pair<ListNodeBase, allocator_type> m_cp_head_tail_link_and_alloc;

      /// as of C++11, all containers size() function complexity must be constant=O(1).
      /// we have to implement a size member for this reason, doing a bit more processing when inserting and erasing
      /// but the size() function should be much faster now.
      /// At time of writing (08/June/2022) it was decided we wouldn't follow this as only the size function would be more performant.
      /// Which is not called very often.
#ifdef REX_ENABLE_SIZE_IN_LISTS
      size_type m_size;
#endif
    };
  } // namespace v1
} // namespace rsl

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <list>

namespace rsl
{
  inline namespace v1
  {
    REX_STD_FUNC_ALIAS(operator==);
    REX_STD_FUNC_ALIAS(operator!=);
    REX_STD_FUNC_ALIAS(operator<);
    REX_STD_FUNC_ALIAS(operator<=);
    REX_STD_FUNC_ALIAS(operator>);
    REX_STD_FUNC_ALIAS(operator>=);

    REX_STD_FUNC_ALIAS(erase);
    REX_STD_FUNC_ALIAS(erase_if);

    REX_STD_FUNC_ALIAS(begin);
    REX_STD_FUNC_ALIAS(cbegin);
    REX_STD_FUNC_ALIAS(cend);
    REX_STD_FUNC_ALIAS(rbegin);
    REX_STD_FUNC_ALIAS(crbegin);
    REX_STD_FUNC_ALIAS(rend);
    REX_STD_FUNC_ALIAS(crend);
    REX_STD_FUNC_ALIAS(size);
    REX_STD_FUNC_ALIAS(ssize);
    REX_STD_FUNC_ALIAS(empty);
    REX_STD_FUNC_ALIAS(data);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"