// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: vector.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/algorithm/in_range.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/algorithm/lexicographical_compare.h"
#include "rex_std/internal/algorithm/max.h"
#include "rex_std/internal/algorithm/remove.h"
#include "rex_std/internal/algorithm/remove_if.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/iterator/distance.h"
#include "rex_std/internal/iterator/random_access_iterator.h"
#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/memory/allocator_traits.h"
#include "rex_std/internal/type_traits/is_trivially_constructible.h"
#include "rex_std/internal/type_traits/is_trivially_destructible.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/move.h"
#include "rex_std/internal/utility/swap.h"
#include "rex_std/iterator.h"

namespace rsl
{
  inline namespace v1
  {

    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Jun/2022)
    // RSL vector also has the numerator and denumerator which used to expand the vector in case of reallocation.
    // by default, the vector's capacity is doubled.
    template <typename T, typename Allocator = rsl::allocator>
    class vector
    {
    private:
      static constexpr card32 realloc_numerator   = 2;
      static constexpr card32 realloc_denumerator = 1;

    public:
      using value_type             = T;
      using size_type              = count_t; /// RSL Comment: Different from ISO C++ Standard at time of writing (26/Jun/2022)
      using difference_type        = int32;
      using allocator_type         = allocator;
      using reference              = value_type&;
      using const_reference        = const value_type&;
      using pointer                = typename rsl::allocator_traits<allocator>::template pointer_or<value_type*>;
      using const_pointer          = typename rsl::allocator_traits<allocator>::template const_pointer_or<const value_type*>;
      using iterator               = random_access_iterator<T>;
      using const_iterator         = const_random_access_iterator<T>;
      using reverse_iterator       = rsl::reverse_iterator<iterator>;
      using const_reverse_iterator = rsl::reverse_iterator<const_iterator>;

      // construction, destruction and assignment.

      // Default constructor. Constructs an empty container with a default-constructed allocator.
      vector()
          : m_begin(nullptr)
          , m_end(nullptr)
          , m_cp_last_and_allocator(nullptr)
      {
      }

      // Constructs an empty container with the given allocator alloc.
      explicit vector(const allocator& alloc)
          : m_begin(nullptr)
          , m_end(nullptr)
          , m_cp_last_and_allocator(nullptr, alloc)
      {
      }
      // Constructs the container with count copies of elements with value value.
      explicit vector(rsl::Size count, const_reference rhs, const allocator& alloc = allocator())
          : vector()
      {
        resize(count.get(), rhs);
      }
      // Constructs the container with count default-inserted instances of T. No copies are made.
      explicit vector(rsl::Size count)
          : vector()
      {
        resize(count.get());
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (26/Jun/2022)
      // Construct the container with a given capacity but size remains 0.
      explicit vector(rsl::Capacity capacity, const allocator& alloc = allocator())
          : vector(alloc)
      {
        reserve(capacity.get());
      }
      // Constructs the container with the contents of the range [first, last).
      template <typename It>
      vector(It first, It last)
          : vector()
      {
        copy_range(first, last);
      }
      // Copy constructor. Constructs the container with the copy of the contents of other.
      vector(const vector& other)
          : vector()
      {
        copy_range(other.cbegin(), other.cend());
      }
      // Constructs the container with the copy of the contents of other, using alloc as the allocator.
      vector(const vector& other, const allocator& alloc)
          : m_begin(nullptr)
          , m_end(nullptr)
          , m_cp_last_and_allocator(nullptr, alloc)
      {
        copy_range(other.cbegin(), other.cend());
      }
      // Move constructor. Constructs the container with the contents of other using move semantics.
      // allocator is obtained by move-construction from the allocator belonging to other.
      // After the move, other is guaranteed to be empty().
      vector(vector&& other) noexcept
          : vector()
      {
        swap(other);
      }
      // allocator-extended move constructor.
      // Using alloc as the allocator for the new container, moving the contents from other;
      // if alloc != other.get_allocator(), this results in an element-wise move.
      // (In that case, other is not guaranteed to be empty after the move.)
      vector(vector&& other, const allocator& alloc)
          : vector()
      {
        if(other.get_allocator() != alloc)
        {
          move_range(other.begin(), other.end());
        }
        else
        {
          swap(other);
        }
      }
      // Constructs the container with the contents of the initializer list init.
      vector(rsl::initializer_list<T> ilist)
          : vector(ilist.begin(), ilist.end())
      {
      }
      // Destructs the vector.
      // The destructors of the elements are called and the used storage is deallocated.
      ~vector()
      {
        clear();      // calls destructor for all elements
        deallocate(); // deallocates heap memory
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (26/Jun/2022)
      // Copy assignment operator. Replaces the contents with a copy of the contents of other.
      vector& operator=(const vector& other)
      {
        REX_ASSERT_X(this, &other, "Can't copy to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in copy assignment, this is not allowed");

        copy_range(other.cbegin(), other.cend());
        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (26/Jun/2022)
      // Move assignment operator. Replaces the contents with those of other using move semantic.
      vector& operator=(vector&& other) noexcept
      {
        REX_ASSERT_X(this, &other, "Can't move to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in move assignment, this is not allowed");

        clear(); // this is not necessary but makes it standard compliant
        swap(other);
        return *this;
      }
      // Replaces the contents with those identified by initializer list ilist.
      vector& operator=(rsl::initializer_list<T> ilist)
      {
        // initializer list has const elements, so we can't move them.
        copy_range(ilist.begin(), ilist.end());
        return *this;
      }

      // Replaces the contents with count copies of value
      void assign(size_type count, const_reference value)
      {
        increase_capacity_if_needed(count);

        for(size_type i = 0; i < count; ++i)
        {
          operator[](i) = value;
        }

        m_end = rsl::max(size(), count);
      }
      // Replaces the contents with copies of those in the range
      void assign(const_iterator begin, const_iterator last)
      {
        copy_range(begin, last);
      }
      // Replaces the contents with the elements from the initializer list.
      void assign(rsl::initializer_list<T> ilist)
      {
        copy_range(ilist.begin(), ilist.end());
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (26/Jun/2022)
      // Returns the allocator associated with the container
      const allocator_type& get_allocator() const
      {
        return m_cp_last_and_allocator.second();
      }

      // element access

      // Returns a reference to the element at specified location pos, with bounds checking.
      // If pos is not within the range of the container, an assert is raised.
      reference at(size_type idx)
      {
        REX_ASSERT_X(idx < size(), "vector index out of range");
        return m_begin[idx];
      }
      // Returns a reference to the element at specified location pos, with bounds checking.
      // If pos is not within the range of the container, an assert is raised.
      const_reference at(size_type idx) const
      {
        REX_ASSERT_X(idx < size(), "vector index out of range");
        return m_begin[idx];
      }
      // Returns a reference to the element at specified location pos. No bounds checking is performed.
      reference operator[](size_type idx)
      {
        return m_begin[idx];
      }
      // Returns a reference to the element at specified location pos. No bounds checking is performed.
      const_reference operator[](size_type idx) const
      {
        return m_begin[idx];
      }
      // Returns a reference to the first element in the container.
      reference front()
      {
        return *m_begin;
      }
      // Returns a reference to the first element in the container.
      const_reference front() const
      {
        return *m_begin;
      }
      // Returns a reference to the last element in the container.
      reference back()
      {
        return *(m_end - 1);
      }
      // Returns a reference to the last element in the container.
      const_reference back() const
      {
        return *(m_end - 1);
      }

      // Returns pointer to the underlying array serving as element storage.
      T* data()
      {
        return m_begin;
      }
      // Returns pointer to the underlying array serving as element storage.
      const T* data() const
      {
        return m_begin;
      }

      // iterators

      // Returns an iterator to the first element of the vector.
      iterator begin()
      {
        return iterator(m_begin);
      }
      // Returns an iterator to the first element of the vector.
      const_iterator begin() const
      {
        return const_iterator(m_begin);
      }
      // Returns an iterator to the first element of the vector.
      const_iterator cbegin() const
      {
        return const_iterator(m_begin);
      }
      // Returns an iterator to the element following the last element of the vector
      iterator end()
      {
        return iterator(m_end);
      }
      // Returns an iterator to the element following the last element of the vector
      const_iterator end() const
      {
        return const_iterator(m_end);
      }
      // Returns an iterator to the element following the last element of the vector
      const_iterator cend() const
      {
        return const_iterator(m_end);
      }

      // Returns a reverse iterator to the first element of the reversed vector.
      // It corresponds to the last element of the non-reversed vector.
      reverse_iterator rbegin()
      {
        return reverse_iterator(end());
      }
      // Returns a reverse iterator to the first element of the reversed vector.
      // It corresponds to the last element of the non-reversed vector.
      const_reverse_iterator rbegin() const
      {
        return reverse_iterator(end());
      }
      // Returns a reverse iterator to the first element of the reversed vector.
      // It corresponds to the last element of the non-reversed vector.
      const_reverse_iterator crbegin() const
      {
        return const_reverse_iterator(cend());
      }
      // Returns a reverse iterator to the element following the last element of the reversed vector.
      // It corresponds to the element preceding the first element of the non-reversed vector.
      // This element acts as a placeholder, attempting to access it results in undefined behavior.
      reverse_iterator rend()
      {
        return reverse_iterator(begin());
      }
      // Returns a reverse iterator to the element following the last element of the reversed vector.
      // It corresponds to the element preceding the first element of the non-reversed vector.
      // This element acts as a placeholder, attempting to access it results in undefined behavior.
      const_reverse_iterator rend() const
      {
        return reverse_iterator(begin());
      }
      // Returns a reverse iterator to the element following the last element of the reversed vector.
      // It corresponds to the element preceding the first element of the non-reversed vector.
      // This element acts as a placeholder, attempting to access it results in undefined behavior.
      const_reverse_iterator crend() const
      {
        return const_reverse_iterator(cbegin());
      }

      // size & capacity

      // Checks if the container has no elements, i.e. whether begin() == end().
      bool empty() const
      {
        return size() == 0;
      }
      // Returns the number of elements in the container, i.e. rsl::distance(begin(), end()).
      size_type size() const
      {
        return static_cast<size_type>(m_end - m_begin);
      }
      // Returns the maximum number of elements the container is able to hold.
      size_type max_size() const
      {
        return rsl::numeric_limits<difference_type>::max();
      }
      // Returns the number of elements that the container has currently allocated space for.
      size_type capacity() const
      {
        return static_cast<size_type>(m_cp_last_and_allocator.first() - m_begin);
      }

      // modifiers
      void resize(size_type newSize)
      {
        const size_type old_size = size();
        if(resize_internal(newSize))
        {
          for(pointer dest = m_begin + old_size; dest != m_end; ++dest)
          {
            new(dest) T();
          }
        }
      }
      void resize(size_type newSize, const_reference value)
      {
        size_type old_size = size();
        if(resize_internal(newSize))
        {
          for(pointer dest = m_begin + old_size; dest != m_end; ++dest)
          {
            new(dest) T(value);
          }
        }
      }
      // Increase the capacity of the vector (the total number of elements that the vector can hold without requiring reallocation)
      void reserve(size_type newCapacity)
      {
        if(newCapacity > capacity())
        {
          reallocate(newCapacity);
        }
      }
      // Removes unused capacity.
      // Note: if unused capacity is found, reallocation does occur.
      void shrink_to_fit()
      {
        // The standard says this is an unbinding function.
        // i.e. it's up to the implementation if reallocation occurs.
        // in RSL, reallocation does occur as it's assumed that if the user calls this function
        // they do not want to waste unused capacity.
        vector tmp = *this;
        swap(tmp);
      }
      // Erases all elements from the container. After this call, size() returns zero.
      void clear()
      {
        if constexpr(!rsl::is_trivially_destructible_v<T>)
        {
          // TODO: should this be in reverse order to be synced with C?
          for(pointer element = m_begin; element < m_end; ++element)
          {
            get_allocator().destroy(element);
          }
        }

        m_end = m_begin;
      }
      // inserts value before pos
      iterator insert(const_iterator pos, const_reference value)
      {
        // prepare_for_new_insert can reallocate, which would invalidate the input it
        difference_type idx = rsl::distance(cbegin(), pos);
        prepare_for_new_insert(pos);
        operator[](idx) = value;
        return (begin() + idx);
      }
      // inserts value before pos
      iterator insert(const_iterator pos, T&& value)
      {
        // prepare_for_new_insert can reallocate, which would invalidate the input it
        const difference_type idx = rsl::distance(cbegin(), pos);
        prepare_for_new_insert(pos);
        operator[](idx) = rsl::move(value);
        return (begin() + idx);
      }
      // inserts count copies of the value before pos
      iterator insert(const_iterator pos, size_type count, const_reference value)
      {
        // prepare_for_new_insert can reallocate, which would invalidate the input it
        difference_type idx     = rsl::distance(cbegin(), pos);
        difference_type end_idx = idx + count;
        prepare_for_new_insert(pos, count);

        for(difference_type i = idx; i < end_idx; ++i)
        {
          operator[](i) = value;
        }
        return iterator(begin() + idx);
      }
      // inserts elements from range [first, last) before pos.
      template <typename It>
      iterator insert(const_iterator pos, It first, It last)
      {
        difference_type count = static_cast<difference_type>(rsl::distance(first, last));
        insert_range(pos, first, last, count);
      }
      // inserts elements from initializer list ilist before pos.
      iterator insert(const_iterator pos, rsl::initializer_list<T> ilist)
      {
        insert_range(pos, ilist.begin(), ilist.end(), ilist.size());
      }
      // Inserts a new element into the container directly before pos
      template <typename... Args>
      iterator emplace(const_iterator pos, Args&&... args)
      {
        // prepare_for_new_insert can reallocate, which would invalidate the input it
        difference_type idx = rsl::distance(cbegin(), pos);
        prepare_for_new_insert(pos);
        operator[](idx) = T(rsl::forward<Args>(args)...);
        return iterator(begin() + idx);
      }
      // Removes the element at pos.
      iterator erase(const_iterator pos)
      {
        auto idx = rsl::distance(cbegin(), pos);
        for(auto i = idx; i < size() - 1; ++i)
        {
          operator[](i) = rsl::move(operator[](i + 1));
        }
        --m_end;
        m_end->~value_type();
        return begin() + idx;
      }
      // Removes the elements in the range [first, last).
      iterator erase(const_iterator first, const_iterator last)
      {
        difference_type count = rsl::distance(first, last);
        auto dst_idx          = rsl::distance(cbegin(), first);
        auto end_idx          = dst_idx + count;
        auto src_idx          = end_idx;

        // make sure the range belongs to the container
        REX_ASSERT_X(rsl::in_range(dst_idx, 0, size()), "Trying to remove a range of elements not belonging to the container.");

        for(auto i = dst_idx; i < end_idx; ++i, ++src_idx)
        {
          operator[](i) = rsl::move(operator[](src_idx));
        }

        if constexpr(!rsl::is_trivially_destructible_v<value_type>)
        {
          auto it = begin() + dst_idx + count;
          for(auto it_end = end(); it != it_end; ++it)
          {
            get_allocator().destroy(rsl::iterator_to_pointer(it));
          }
        }

        m_end -= count;
        return begin() + dst_idx;
      }
      // Adds the given element value at the end of the container.
      // The new element is initialized as a copy of obj.
      void push_back(const_reference obj)
      {
        prepare_for_new_element();
        new(m_end) T(obj);
        ++m_end;
      }
      // Adds the given element value at the end of the container.
      // obj is moved into the new element.
      void push_back(T&& obj)
      {
        prepare_for_new_element();
        new(m_end) T(rsl::move(obj));
        ++m_end;
      }

      // Appends a new element to the end of the container.
      // the element is initalized with the arguments provided.
      template <typename... Args>
      reference emplace_back(Args&&... args)
      {
        prepare_for_new_element();

        new(m_end) T(rsl::forward<Args>(args)...);
        ++m_end;

        return back();
      }
      // Removes the last element of the container.
      void pop_back()
      {
        back().~T();
        --m_end;
      }

      // Exchanges the contents of the container with those of other.
      // Does not invoke any move, copy, or swap operations on individual elements.
      void swap(vector& other)
      {
        rsl::swap(m_begin, other.m_begin);
        rsl::swap(m_end, other.m_end);
        rsl::swap(m_cp_last_and_allocator, other.m_cp_last_and_allocator);
      }

    private:
      // Allocates a bigger buffer and copies over all elements
      // Also sets m_begin, m_end and m_cp_last_and_allocator.first() to point to the new buffer
      void reallocate(size_type newCapacity)
      {
        pointer new_buffer = static_cast<pointer>(get_allocator().allocate(calc_bytes_needed(newCapacity)));

        pointer dest = new_buffer;
        move(dest, m_begin, size());

        const size_type old_size = size();

        clear(); // to call the dtor of non trivially destructible types
        deallocate();

        m_begin                         = new_buffer;
        m_end                           = m_begin + old_size;
        m_cp_last_and_allocator.first() = m_begin + newCapacity;
      }

      // Deallocates the buffer
      void deallocate()
      {
        get_allocator().deallocate(data());
      }

      // calls reallocate if new required size is bigger than capacity
      bool increase_capacity_if_needed(size_type newRequiredCapacity)
      {
        if(newRequiredCapacity > capacity())
        {
          const size_type num_elements_to_add = newRequiredCapacity - capacity();
          reallocate(new_buffer_capacity(num_elements_to_add));
          return true;
        }
        return false;
      }
      // returns the size of a new buffer on reallocation
      size_type new_buffer_capacity(size_type numElementsToAdd) const
      {
        return (capacity() * realloc_numerator / realloc_denumerator) + numElementsToAdd;
      }

      // reallocates if we surpassed our capacity with the new element
      void prepare_for_new_element()
      {
        increase_capacity_if_needed(size() + 1);
      }

      // moves every element starting at 'pos' one space to the right
      void prepare_for_new_insert(const_iterator pos, size_type count = 1)
      {
        const size_type idx = rsl::distance(cbegin(), pos);

        if(size() + count > capacity())
        {
          // This is similar to what basic_string does.
          // Please look at basic_string::prepare_for_new_insert if you want to have more
          // documentation about what goes on here

          const size_type size_for_new_buffer = new_buffer_capacity(count);
          pointer new_buffer                  = static_cast<pointer>(get_allocator().allocate(calc_bytes_needed(size_for_new_buffer)));

          // move the first set of elements up to pos into the new buffer
          pointer dest  = new_buffer;
          pointer start = m_begin;
          move(dest, start, idx);

          // move the second set of elements from pos to end() into the new buffer
          start = start + idx;
          dest += idx + count;
          move(dest, start, size() - idx);

          // all that's left is to deallocate the old buffer
          deallocate();

          // reset the data pointers.
          m_begin                         = new_buffer;
          m_end                           = m_begin + size_for_new_buffer;
          m_cp_last_and_allocator.first() = m_begin + size_for_new_buffer;
        }
        else
        {
          // using memmove here as the dst and src could overlap
          pointer dst_in_buffer = data() + idx + count;
          move(dst_in_buffer, data() + idx, size() - idx);
        }
      }

      // resizes if necessary
      // copies the range to the internal element starting at m_begin
      template <typename It>
      void copy_range(It begin, It last)
      {
        const difference_type count = static_cast<difference_type>(rsl::distance(begin, last));
        resize_internal(count);

        for(difference_type i = 0; i < count; ++i)
        {
          T* dest = rsl::addressof(operator[](i));
          new(dest) T(*(begin + i));
        }
      }

      // resizes if necessary
      // moves the range to the internal element starting at m_begin
      template <typename It>
      void move_range(It begin, It last)
      {
        difference_type count = static_cast<difference_type>(rsl::distance(begin, last));
        resize_internal(count);

        for(difference_type i = 0; i < count; ++i)
        {
          T* dest = rsl::addressof(operator[](i));
          new(dest) T(rsl::move(*(begin + i)));
        }
      }

      // reallocates if 'new_size' is bigger than the current capacity
      // sets m_end so it matches our size
      // returns true if a resize occurred
      bool resize_internal(card32 newSize)
      {
        increase_capacity_if_needed(newSize);
        const difference_type old_size = size();
        m_end                          = m_begin + newSize;
        return newSize > old_size;
      }

      size_type calc_bytes_needed(card32 newElementCount)
      {
        return sizeof(T) * newElementCount;
      }

      template <typename InputIt>
      void insert_range(iterator pos, InputIt first, InputIt last, size_type count)
      {
        // prepare_for_new_insert can reallocate, which would invalidate the input it
        difference_type idx     = static_cast<difference_type>(rsl::distance(cbegin(), pos));
        difference_type end_idx = idx + count;
        prepare_for_new_insert(pos, count);
        auto it = first;
        for(difference_type i = idx; i < end_idx; ++i)
        {
          operator[](i) = *it;
          ++it;
        }
      }

      void copy(pointer dst, const_pointer src, size_type count)
      {
        while(count > 0)
        {
          new(dst) T(*src);
          ++dst;
          ++src;
          --count;
        }
      }
      void move(pointer dst, pointer src, size_type count)
      {
        while(count > 0)
        {
          new(dst) T(rsl::move(*src));
          ++dst;
          ++src;
          --count;
        }
      }

      allocator_type& get_allocator()
      {
        return m_cp_last_and_allocator.second();
      }

    private:
      pointer m_begin;
      pointer m_end;
      rsl::compressed_pair<pointer, allocator> m_cp_last_and_allocator;
    };

    // Checks if the contents of lhs and rhs are equal, that is, they have the same number of elements and each element in lhs compares equal with the element in rhs at the same position.
    template <typename T, typename Alloc>
    bool operator==(const rsl::vector<T, Alloc>& lhs, const rsl::vector<T, Alloc>& rhs)
    {
      if(lhs.size() != rhs.size())
      {
        return false;
      }

      for(auto lhs_it = lhs.begin(), rhs_it = rhs.begin(), lhs_end = lhs.end(); lhs_it != lhs_end; ++lhs_it, ++rhs_it)
      {
        if(*lhs_it != *rhs_it)
        {
          return false;
        }
      }

      return true;
    }
    // Checks if the contents of lhs and rhs are not equal, that is, they have a different number of elements or
    // there is an element mismatch between the 2 iterators
    template <typename T, typename Alloc>
    bool operator!=(const rsl::vector<T, Alloc>& lhs, const rsl::vector<T, Alloc>& rhs)
    {
      return !(lhs == rhs);
    }
    // Compares the contents of lhs and rhs lexicographically
    template <typename T, typename Alloc>
    bool operator<(const rsl::vector<T, Alloc>& lhs, const rsl::vector<T, Alloc>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) < 0;
    }
    // Compares the contents of lhs and rhs lexicographically
    template <typename T, typename Alloc>
    bool operator<=(const rsl::vector<T, Alloc>& lhs, const rsl::vector<T, Alloc>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) <= 0;
    }
    // Compares the contents of lhs and rhs lexicographically
    template <typename T, typename Alloc>
    bool operator>(const rsl::vector<T, Alloc>& lhs, const rsl::vector<T, Alloc>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) > 0;
    }
    // Compares the contents of lhs and rhs lexicographically
    template <typename T, typename Alloc>
    bool operator>=(const rsl::vector<T, Alloc>& lhs, const rsl::vector<T, Alloc>& rhs)
    {
      return rsl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) >= 0;
    }
    // Specializes the rsl::swap algorithm for rsl::vector. Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
    template <typename T, typename Alloc>
    void swap(rsl::vector<T, Alloc>& lhs, rsl::vector<T, Alloc>& rhs)
    {
      lhs.swap(rhs);
    }
    // Erases all elements that compare equal to value from the container.
    template <typename T, typename Alloc, typename U>
    typename rsl::vector<T, Alloc>::size_type erase(rsl::vector<T, Alloc>& c, const U& value)
    {
      auto it = rsl::remove(c.begin(), c.end(), value);
      auto r  = rsl::distance(it, c.end());
      c.erase(it, c.end());
      return r;
    }
    // Erases all elements that satisfy the predicate pred from the container.
    template <typename T, typename Alloc, typename Pred>
    typename rsl::vector<T, Alloc>::size_type erase_if(rsl::vector<T, Alloc>& c, Pred pred)
    {
      auto it = rsl::remove_if(c.begin(), c.end(), pred);
      auto r  = rsl::distance(it, c.end());
      c.erase(it, c.end());
      return r;
    }

    // This deduction guide is provided for vector to allow deduction from an iterator range.
    template <typename InputIt, typename Alloc = rsl::allocator>
    vector(InputIt, InputIt, Alloc = Alloc()) -> vector<typename rsl::iterator_traits<InputIt>::value_type, Alloc>;

  } // namespace v1
} // namespace rsl

#include "rex_std/internal/assert/assert_impl.h"