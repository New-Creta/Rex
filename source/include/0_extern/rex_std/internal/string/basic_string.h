// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_string.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/string/character_lookup.h"
#include "rex_std/bonus/string/stack_string.h"
#include "rex_std/bonus/string/string_utils.h"
#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/bonus/utility/element_literal.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/algorithm/memcmp.h"
#include "rex_std/internal/algorithm/remove.h"
#include "rex_std/internal/algorithm/remove_if.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/functional/hash.h"
#include "rex_std/internal/iterator/begin.h"
#include "rex_std/internal/iterator/end.h"
#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/iterator/iterator_traits.h"
#include "rex_std/internal/math/abs.h"
#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/memory/allocator_traits.h"
#include "rex_std/internal/memory/byte.h"
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/internal/memory/memmove.h"
#include "rex_std/internal/memory/memset.h"
#include "rex_std/internal/string/byte_strings.h"
#include "rex_std/internal/string/char_traits.h"
#include "rex_std/internal/string/string_forward_declare.h"
#include "rex_std/istream.h"
#include "rex_std/ostream.h"
#include "rex_std/stddef.h"

// Strings are in theory just fancy vectors.
// They act like a vector of chars, but have extra functionality,
// however their internal implementation is a bit different.
// Where vector only needs to hold 3 pointers, "begin", "end" and "last"
// with "end" and "last" being the same if size() == capacity(),
// with a string, "end" will point to the null termination char
// and "last" will point to the element following end

namespace rsl
{
  inline namespace v1
  {

    template <typename CharType, typename Traits>
    class basic_string_view;

    template <typename CharType, typename Traits = char_traits<CharType>, typename Alloc = allocator>
    class basic_string
    {
    public:
      using traits_type     = Traits;
      using value_type      = CharType;
      using allocator_type  = allocator;
      using size_type       = count_t; /// RSL Comment: Different from ISO C++ Standard at time of writing (28/Jun/2022)
      using difference_type = int32;   /// RSL Comment: Different from ISO C++ Standard at time of writing (28/Jun/2022)
      using reference       = value_type&;
      using const_reference = const value_type&;
      using pointer         = typename rsl::allocator_traits<allocator>::template pointer_or<value_type*>;
      using const_pointer   = typename rsl::allocator_traits<allocator>::template const_pointer_or<const value_type*>;

      using iterator               = random_access_iterator<value_type>;
      using const_iterator         = const_random_access_iterator<value_type>;
      using reverse_iterator       = rsl::reverse_iterator<iterator>;
      using const_reverse_iterator = rsl::reverse_iterator<const_iterator>;

      static constexpr size_type s_sso_buff_size = 16;
      static constexpr size_type s_npos          = static_cast<size_type>(-1);

      // Default constructor. Constructs empty string
      basic_string()
          : m_sso_buffer()
          , m_begin(m_sso_buffer.data())
          , m_end(m_sso_buffer.data())
          , m_cp_last_and_allocator(m_sso_buffer.data() + m_sso_buffer.max_size())
      {
      }
      // Default constructor. Constructs an empty string
      explicit basic_string(const allocator& alloc)
          : m_sso_buffer()
          , m_begin(m_sso_buffer.data())
          , m_end(m_sso_buffer.data())
          , m_cp_last_and_allocator(m_sso_buffer.data() + m_sso_buffer.max_size(), alloc)
      {
      }
      // Constructs the string with count copies of character ch
      basic_string(size_type count, value_type ch, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        assign(count, ch);
      }
      // Constructs the string with a substring of other. starting at pos and going to the end of other
      basic_string(const basic_string& other, size_type pos, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        REX_ASSERT_X(other.size() - pos >= 0, "invalid pos value given to string ctor");
        assign(rsl::iterator_to_pointer(other.begin() + pos), other.size() - pos);
      }
      // Constructs the string with a substring of other. starting at pos and going to count
      basic_string(const basic_string& other, size_type pos, size_type count, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        assign(rsl::iterator_to_pointer(other.cbegin() + pos), (rsl::min)(count, other.size()));
      }
      // Construct a string from s ranging from s to s + count.
      basic_string(const_pointer s, size_type count, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        assign(s, count);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (28/Jun/2022)
      // by adding a reference to the const_pointer overload, this one will only get selected if the argument
      // is a pointer type, and array arguments will get passed to the string_view overload ctor
      explicit basic_string(const_pointer& s, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        const size_type length = traits_type::length(s);
        assign(s, length);
      }
      // constructs the string with the contents of the range [first, last)
      template <typename InputIt>
      basic_string(InputIt first, InputIt last, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        assign(first, last);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (28/Jun/2022)
      // marked explicit in RSL
      // copy constructor. copies the contents of other.
      explicit basic_string(const basic_string& other) // NOLINT(google-explicit-constructor)
          : basic_string(other.get_allocator())
      {
        assign(other.data(), other.length());
      }
      // construct the string with the contents of other, using the allocator provided
      basic_string(const basic_string& other, const allocator& alloc)
          : basic_string(alloc)
      {
        assign(other.data(), other.length());
      }
      // move constructor, swaps contents of other with the current string
      basic_string(basic_string&& other)
          : basic_string(rsl::move(other.get_allocator()))
      {
        swap(other);
      }
      // swaps the contents of other with the current string if allocs are equal
      // copies the contents over otherwise using the allocator provided
      // this overload is not necessary, but if it's removed, then the following code
      // rsl::string str;
      // rsl::string str2(rsl::move(str), rsl::allocator);
      // would call the copy constructor with a new allocator, which is not what we want.
      basic_string(basic_string&& other, const allocator& alloc)
          : basic_string(alloc)
      {
        REX_ASSERT_X(other.get_allocator() == alloc, "different allocators in move constructor, this is not allowed");
        swap(other);
      }
      // Construct the string from the given initializer_list
      basic_string(rsl::initializer_list<value_type> ilist, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        // using insert_n instead of insert because we know the length
        // and can therefore allocate the length straight away and then fill
        // in the buffer.
        // This avoid possible reallocation
        assign(ilist);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (28/Jun/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // Constructs the string using the contents of the string view
      explicit basic_string(const basic_string_view<CharType>& sv, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        // using insert_n instead of insert because we know the length
        // and can therefore allocate the length straight away and then fill
        // in the buffer.
        // This avoid possible reallocation
        assign(sv.data(), sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (28/Jun/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // Constructs the string using the contents of the string view
      // starting at pos and going till pos + n
      explicit basic_string(const basic_string_view<value_type, traits_type> sv, size_type pos, size_type n, const allocator& alloc = allocator())
          : basic_string(alloc)
      {
        assign(sv.data() + pos, n);
      }
      // A string cannot be constructed from a nullptr
      basic_string(rsl::nullptr_t) = delete;

      // Deallocates all memory the string uses.
      ~basic_string()
      {
        deallocate();
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (30/Jun/2022)
      // in RSL, you can't copy containers if the allocators differ.
      // copy assignment. the contents of other are copied into this.
      basic_string& operator=(const basic_string& other)
      {
        REX_ASSERT_X(this, &other, "Can't copy to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in copy assignment, this is not allowed");

        assign(other.data(), other.length());
        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (30/Jun/2022)
      // in RSL, you can't move containers if the allocators differ.
      // move assignment. swaps the members of the containers.
      // no allocation is performed
      basic_string& operator=(basic_string&& other) noexcept
      {
        REX_ASSERT_X(this, &other, "Can't move to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in move assignment, this is not allowed");

        move_assign(rsl::move(other));
        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (15/Oct/2022)
      // implicit assigning from a pointer is not supported because this makes it easier to
      // allocate heap memory. A user should use the "assign" function to do so.
      // the overload to assign from a string_view is not defined for the same reason
      // template <count_t Size>
      // basic_string& operator=(const_pointer (&str)[Size]);
      // basic_string& operator=(const_pointer str);
      // basic_string& operator=(const rsl::basic_string_view<CharType>& view);
      //
      // copies the contents of the string by replacing the entire array with a single char
      basic_string& operator=(value_type ch)
      {
        return assign(rsl::addressof(ch), 1); // NOLINT(misc-unconventional-assign-operator)
      }

      // a string cannot be assigned from a nullptr.
      basic_string& operator=(rsl::nullptr_t) = delete;

      // replaces the contents with count copies of character
      basic_string& assign(size_type count, value_type ch)
      {
        const size_type new_size = count + 1;

        if(new_size > capacity())
        {
          deallocate();
          pointer new_buffer = static_cast<pointer>(get_allocator().allocate(new_size));
          reset(new_buffer, new_size, new_size);
        }

        traits_type::assign(m_begin, count, ch);
        m_end = m_begin + count;
        traits_type::assign(*m_end, value_type());
        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // in RSL, you can't assign to a container if the allocators differ.
      // replaces the contents with a copy of str, equivalent to the copy assignment operator
      basic_string& assign(const basic_string& other)
      {
        REX_ASSERT_X(this, &other, "Can't assign to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in assignment, this is not allowed");

        assign(other.data(), other.length());
        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing time of writing (01/Jul/2022)
      // in RSL, you can't assign to a container if the allocators differ.
      // replaces the contents with a copy of str, starting at pos and going till pos + count
      basic_string& assign(const basic_string& other, size_type pos, size_type count = s_npos)
      {
        REX_ASSERT_X(this, &other, "Can't assign to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in assignment, this is not allowed");

        const size_type num_to_copy = obj_length_or_count(other, count, pos);
        assign(other.data() + pos, num_to_copy);
        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // in RSL, you can't assign to a container if the allocators differ.
      // replaces the contents with the content of str. equivalent to the move assignment operator
      basic_string& assign(basic_string&& other)
      {
        REX_ASSERT_X(this, &other, "Can't assign to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in assignment, this is not allowed");

        move_assign(rsl::move(other));
        return *this;
      }
      // Replaces the contents with the content of str till str + count.
      basic_string& assign(const_pointer str, size_type count)
      {
        if(is_using_sso_string() && count < s_sso_buff_size)
        {
          sso_assign(str, count);
        }
        else
        {
          heap_assign(str, count);
        }

        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing time of writing (30/Jun/2022)
      // the standard doesn't provide an overload for a string literal
      // replaces the contents of the string array
      basic_string& assign(const_pointer& str) // NOLINT(modernize-avoid-c-arrays)
      {
        return assign(str, Size);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (30/Jun/2022)
      // Because we have the above function defined, we don't overload the assign function
      // taking just a const_pointer.
      // basic_string& assign(const_pointer str);

      // Replaces the contents with copies of the characters in range [first, last)
      template <typename InputIt>
      basic_string& assign(InputIt first, InputIt last)
      {
        // you don't know the size and iterator could be forward iterator
        // so this is very slow as it has to check for a possible resize after every insertion.
        while(first != last)
        {
          insert(cend(), *first);
          ++first;
        }

        return *this;
      }
      // replaces the contents with those of the initializer list
      basic_string& assign(rsl::initializer_list<value_type> ilist)
      {
        // pretty similar to the assign using iterators
        // but we know the size here, so we can allocate everything up front
        // and then copy into the string

        // don't use reserve here as it'd copy the current values over.
        // we'll overwrite them anyway so there's no point in doing that
        const size_type new_size = static_cast<size_type>(ilist.size()) + 1; // +1 for the null termination char

        if(new_size > capacity())
        {
          deallocate();
          pointer new_buffer = static_cast<pointer>(get_allocator().allocate(new_size));
          reset(new_buffer, new_size, new_size);
        }

        pointer dst = m_begin;
        for(const value_type c: ilist)
        {
          traits_type::assign(*dst, c);
          ++dst;
        }
        m_end = dst;
        traits_type::assign(*m_end, value_type());

        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (30/Jun/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // replaces the contents with the content of the string view.
      basic_string& assign(basic_string_view<value_type, traits_type> sv)
      {
        return assign(sv.data(), sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (30/Jun/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // start at pos and going till pos + count.
      basic_string& assign(basic_string_view<value_type, traits_type> sv, size_type pos, size_type count = s_npos)
      {
        const size_type num_to_copy = obj_length_or_count(sv, count, pos);
        return assign(sv.data() + pos, num_to_copy);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // returns the allocator used by the string
      const allocator_type& get_allocator() const
      {
        return m_cp_last_and_allocator.second();
      }

      //
      // element access
      //

      // Returns a reference to the character at specified location pos
      // bounds checking is performed
      reference at(size_type pos)
      {
        REX_ASSERT_X(pos < size(), "string out of range");
        return m_begin[pos];
      }
      // Returns a reference to the character at specified location pos
      // bounds checking is performed
      const_reference at(size_type pos) const
      {
        REX_ASSERT_X(pos < size(), "string out of range");
        return m_begin[pos];
      }
      // Returns a reference to the character at specified location pos
      reference operator[](size_type idx)
      {
        return m_begin[idx];
      }
      // Returns a reference to the character at specified location pos
      const_reference operator[](size_type idx) const
      {
        return m_begin[idx];
      }
      // Returns a reference to the first element in the string
      reference front()
      {
        return operator[](0);
      }
      // Returns a reference to the first element in the string
      const_reference front() const
      {
        return operator[](0);
      }
      // Returns a reference to the last element in the string
      reference back()
      {
        return operator[](size() - 1);
      }
      // Returns a reference to the last element in the string
      const_reference back() const
      {
        return operator[](size() - 1);
      }
      // Returns a pointer to the underlying array serving as character storage
      pointer data()
      {
        return m_begin;
      }
      // Returns a pointer to the underlying array serving as character storage
      const_pointer data() const
      {
        return m_begin;
      }
      // Returns a pointer to a null-terminated character array with data equivalent to those stored in the string
      const_pointer c_str() const
      {
        return m_begin;
      }
      // returns a basic_string_view<value_type, traits_type> pointing to the current string
      operator basic_string_view<value_type, traits_type>() const // NOLINT(google-explicit-constructor)
      {
        return basic_string_view<value_type, traits_type>(data(), size());
      }

      // iterators

      // returns an iterator to the first character of the string
      iterator begin()
      {
        return iterator(m_begin);
      }
      // returns an iterator to the first character of the string
      const_iterator begin() const
      {
        return const_iterator(m_begin);
      }
      // returns an iterator to the first character of the string
      const_iterator cbegin() const
      {
        return const_iterator(m_begin);
      }
      // returns an iterator to the character following the last character of the string
      iterator end()
      {
        return iterator(m_end);
      }
      // returns an iterator to the character following the last character of the string
      const_iterator end() const
      {
        return const_iterator(m_end);
      }
      // returns an iterator to the character following the last character of the string
      const_iterator cend() const
      {
        return const_iterator(m_end);
      }
      // returns a reverse iterator to the first character of the reversed string
      reverse_iterator rbegin()
      {
        return reverse_iterator(end());
      }
      // returns a reverse iterator to the first character of the reversed string
      reverse_iterator rbegin() const
      {
        return reverse_iterator(end());
      }
      // returns a reverse iterator to the first character of the reversed string
      const_reverse_iterator crbegin() const
      {
        return const_reverse_iterator(cend());
      }
      // returns a reverse iterator to the character following the last character of the reversed string
      reverse_iterator rend()
      {
        return reverse_iterator(begin());
      }
      // returns a reverse iterator to the character following the last character of the reversed string
      reverse_iterator rend() const
      {
        return reverse_iterator(begin());
      }
      // returns a reverse iterator to the character following the last character of the reversed string
      const_reverse_iterator crend() const
      {
        return const_reverse_iterator(cbegin());
      }

      //
      // capacity
      //
      // returns if the string is empty.
      bool empty() const
      {
        return size() == 0;
      }
      // returns the length of the string
      size_type size() const
      {
        return static_cast<size_type>(m_end - m_begin);
      }
      // returns the length of the string
      size_type length() const
      {
        return size();
      }
      // returns the maximum allowed elements in the string
      size_type max_size() const
      {
        return rsl::numeric_limits<difference_type>::max();
      }
      // request memory reallocation so the string can hold "new_capacity" amount of characters
      void reserve(size_type newCapacity)
      {
        if(newCapacity > capacity())
        {
          reallocate(newCapacity, data(), size());
        }
      }
      // returns the number of characters the string can currently hold
      size_type capacity() const
      {
        return static_cast<size_type>(last() - m_begin);
      }
      // request the removal of unused capacity.
      void shrink_to_fit()
      {
        if(is_using_sso_string())
        {
          return;
        }

        reallocate(size() + 1, data(), size());
      }
      // removes all characters from the string string
      void clear()
      {
        m_end = m_begin;
      }

      // Inserts count copies of ch at position index
      basic_string& insert(size_type index, size_type count, value_type ch)
      {
        prepare_for_new_insert(index, count);

        pointer offset_loc = data() + index;
        traits_type::assign(offset_loc, count, ch);

        return *this;
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (30/Jun/2022)
      // the standard doesn't provide an overload for a string literal
      // Inserts character string pointed to by s at the position index
      template <count_t Size>
      basic_string& insert(size_type index, const value_type (&s)[Size]) // NOLINT(modernize-avoid-c-arrays)
      {
        return insert(index, s, Size - 1);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // because we have the function above, we don't specify the following function
      // basic_string& insert(size_type index, const_pointer s);

      // inserts the character range [s, s + count) at position index
      basic_string& insert(size_type index, const_pointer s, size_type count)
      {
        prepare_for_new_insert(index, count);

        pointer offset_loc = data() + index;
        traits_type::copy(offset_loc, s, count);

        return *this;
      }
      // insert string str at position index
      basic_string& insert(size_type index, const basic_string& str)
      {
        return insert(index, str.data(), str.size());
      }
      // inserts a string obtained by str.substr(index_str, count) at the position index
      basic_string& insert(size_type index, const basic_string& str, size_type indexStr, size_type count = s_npos)
      {
        const size_type num_to_copy = obj_length_or_count(str, count, indexStr);
        return insert(index, str.data(), num_to_copy);
      }
      // inserts character ch before the character pointed by pos
      iterator insert(const_iterator pos, value_type ch)
      {
        return insert(pos, 1, ch);
      }
      // inserts count copies of character ch before the element (if any) pointed by pos
      iterator insert(const_iterator pos, size_type count, value_type ch)
      {
        const size_type pos_idx = rsl::distance(cbegin(), pos);

        prepare_for_new_insert(pos_idx, count);

        traits_type::assign(&m_begin[pos_idx], count, ch);

        return iterator(&m_begin[pos_idx]);
      }
      // inserts characters from teh range [first, last) before the element (if any) pointed by pos.
      template <typename InputIt>
      iterator insert(const_iterator pos, InputIt first, InputIt last)
      {
        const size_type pos_idx                = rsl::distance(cbegin(), pos);
        const size_type num_elements_to_insert = rsl::distance(first, last);

        prepare_for_new_insert(pos_idx, num_elements_to_insert);

        size_type current_idx = pos_idx;
        while(first != last)
        {
          traits_type::assign(operator[](current_idx), *first);
          ++current_idx;
          ++first;
        }
        return iterator(&m_begin[current_idx]);
      }
      // inserts elements from the initializer list before the element (if any) pointed by pos
      iterator insert(const_iterator pos, rsl::initializer_list<value_type> ilist)
      {
        const size_type pos_idx                = rsl::distance(cbegin(), pos);
        const size_type num_elements_to_insert = static_cast<size_type>(ilist.size());

        prepare_for_new_insert(pos, num_elements_to_insert);

        size_type current_idx = pos_idx;
        for(const value_type ch: ilist)
        {
          traits_type::assign(operator[](current_idx), ch);
          ++current_idx;
        }

        return iterator(&m_begin[current_idx]);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // inserts the elements from sv before the element (if any) pointed by pos.
      basic_string& insert(size_type pos, const basic_string_view<value_type, traits_type>& sv)
      {
        return insert(pos, sv.data(), sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // inserts the elements from sv starting at index_str before the element (if any) pointed by pos.
      basic_string& insert(size_type pos, const basic_string_view<value_type, traits_type>& sv, size_type indexStr, size_type count = s_npos)
      {
        const size_type num_chars_to_insert = obj_length_or_count(sv, count, indexStr);
        return insert(pos, sv.data() + indexStr, num_chars_to_insert);
      }

      // removes min(count, size() - index) characters starting at index
      basic_string& erase(size_type index = 0, size_type count = s_npos)
      {
        const size_type num_to_erase = count != s_npos ? (rsl::min)(size() - index, count) : size() - index;

        traits_type::move(&m_begin[index], &m_begin[index + num_to_erase], (size() - (index + num_to_erase)));

        m_end = &m_begin[size() - num_to_erase];
        traits_type::assign(*m_end, value_type());

        return *this;
      }
      // removes the character at position
      iterator erase(const_iterator position)
      {
        const size_type pos_idx = rsl::distance(cbegin(), position);

        traits_type::move(&m_begin[pos_idx], &m_begin[pos_idx + 1], size() - pos_idx);

        --m_end;
        traits_type::assign(*m_end, value_type());

        return iterator(&m_begin[pos_idx]);
      }
      // removes the characters in the range [frist, last)
      iterator erase(const_iterator first, const_iterator last)
      {
        const size_type first_idx = rsl::distance(cbegin(), first);
        const size_type last_idx  = rsl::distance(cbegin(), last);
        const size_type count     = last_idx - first_idx;

        traits_type::move(&m_begin[first_idx], &m_begin[last_idx], count);

        m_end -= count;
        traits_type::assign(*m_end, value_type());

        return iterator(&m_begin[first_idx]);
      }

      // appends the given character to the end of the string
      void push_back(value_type ch)
      {
        increase_capacity_if_needed(1_elem);

        traits_type::assign(*m_end, ch);
        ++m_end;
        traits_type::assign(*m_end, value_type());
      }

      // removes the last character from the string
      void pop_back()
      {
        --m_end;
        traits_type::assign(*m_end, value_type());
      }

      // appends count copies of character ch
      basic_string& append(size_type count, value_type ch)
      {
        increase_capacity_if_needed(count);
        traits_type::assign(m_end, count, ch);
        m_end += count;
        traits_type::assign(*m_end, value_type());

        return *this;
      }
      // appends string str
      basic_string& append(const basic_string& str)
      {
        return append(str.data(), str.size());
      }
      // appends a substr [pos, pos + count) of str.
      basic_string& append(const basic_string& str, size_type pos, size_type count = s_npos)
      {
        return append(str.data() + pos, obj_length_or_count(str, count, pos));
      }
      // appends characters in the range [s, s + count)
      basic_string& append(const_pointer s, size_type count)
      {
        increase_capacity_if_needed(count);
        traits_type::copy(m_end, s, count);
        m_end += count;
        traits_type::assign(*m_end, value_type());

        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (30/Jun/2022)
      // this takes a pointer reference to let literal strings overload to the string view overload.
      basic_string& append(const_pointer& s) // NOLINT(modernize-avoid-c-arrays)
      {
        return append(s, traits_type::length(s));
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // Because we have the above function, we don't specify the following.
      // basic_string& append(const_pointer s);

      // appends characters in the range [first, last)
      template <typename InputIt>
      basic_string& append(InputIt first, InputIt last)
      {
        const size_type count = rsl::distance(first, last);

        increase_capacity_if_needed(count);

        while(first != last)
        {
          traits_type::assign(*m_end, *first);
          ++m_end;
          ++first;
        }

        traits_type::assign(*m_end, value_type());

        return *this;
      }
      // appends characters from the initializer list
      basic_string& append(rsl::initializer_list<value_type> ilist)
      {
        increase_capacity_if_needed(static_cast<size_type>(ilist.size()));

        for(const value_type ch: ilist)
        {
          traits_type::assign(*m_end, ch);
          ++m_end;
        }

        traits_type::assign(*m_end, value_type());

        return *this;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // appends all characters from sv
      basic_string& append(const basic_string_view<value_type, traits_type> sv)
      {
        return append(sv.data(), sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // appends all characters from the range [pos, count) from sv.
      basic_string& append(const basic_string_view<value_type, traits_type>& sv, size_type pos, size_type count = s_npos)
      {
        return append(sv.data() + pos, obj_length_or_count(sv, count, pos));
      }

      // Appends string str
      basic_string& operator+=(const basic_string& str)
      {
        return append(str);
      }
      // Appends character ch
      basic_string& operator+=(value_type ch)
      {
        push_back(ch);
        return *this;
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (30/Jun/2022)
      // the standard doesn't provide an overload for a string literal
      // Appends character string pointed to by s.
      template <count_t Size>
      basic_string& operator+=(const value_type (&s)[Size]) // NOLINT(modernize-avoid-c-arrays)
      {
        return append(s, Size - 1);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (30/Jun/2022)
      // Because we have the above function, we don't specify the following.
      // basic_string& operator+=(const_pointer s);

      // appends the characters from the initializer list
      basic_string& operator+=(rsl::initializer_list<value_type> ilist)
      {
        return append(ilist);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // appends characters in the string string view
      basic_string& operator+=(const basic_string_view<value_type, traits_type>& sv)
      {
        return append(sv);
      }

      // comparison and queries

      // compares this string to str
      REX_NO_DISCARD int32 compare(const basic_string& str) const
      {
        return Traits::compare(data(), str.data());
      }
      // compares a [pos1, pos1+count1) substring of this string to str
      REX_NO_DISCARD int32 compare(size_type pos1, size_type count1, const basic_string& str) const
      {
        return internal::string_utils::compare<traits_type>(data() + pos1, str.data(), count1, str.length());
      }
      // compares a [pos1, pos1+count1) substring of this string to a substring [pos2, pos2+count2) of str
      REX_NO_DISCARD int32 compare(size_type pos1, size_type count1, const basic_string& str, size_type pos2, size_type count2) const
      {
        return internal::string_utils::compare<traits_type>(data() + pos1, str.data() + pos2, count1, count2);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (30/Jun/2022)
      // the standard doesn't provide an overload for a string literal
      // Compares the string to a string literal.
      template <count_t Size>
      REX_NO_DISCARD int32 compare(const_pointer (&s)[Size]) const // NOLINT(modernize-avoid-c-arrays)
      {
        return Traits::compare(data(), s);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // Because we have the above function, we don't specify the following.
      // int32 compare(const_pointer s) const;

      /// RSL Comment: Not in ISO C++ Standard at time of writing (01/Jul/2022)
      // the standard doesn't provide an overload for a string literal
      // compares a [pos1, pos1_count1) substring of this string to a string literal
      template <count_t Size>
      REX_NO_DISCARD int32 compare(size_type pos1, size_type count1, const_pointer (&s)[Size]) const // NOLINT(modernize-avoid-c-arrays)
      {
        return internal::string_utils::compare<traits_type>(data() + pos1, s, count1, Size - 1);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // Because we have the above function, we don't specify the following.
      // int32 compare(size_type pos1, size_type count1, const_pointer s) const;

      // compares a [pos1, pos1+count1) substring of this string to another string pointed
      // to by s with a length of count2.
      REX_NO_DISCARD int32 compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const
      {
        return internal::string_utils::compare<traits_type>(data() + pos1, s, count1, count2);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // compares this string to sv
      REX_NO_DISCARD int32 compare(const basic_string_view<value_type, traits_type>& sv) const
      {
        return internal::string_utils::compare<traits_type>(data(), sv.data(), length(), sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // compares [pos1, pos1+count1) substring of this string to sv
      REX_NO_DISCARD int32 compare(size_type pos1, size_type count1, const basic_string_view<value_type, traits_type> sv) const
      {
        return internal::string_utils::compare<traits_type>(data() + pos1, sv.data(), count1, sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // compares [pos1, pos1+count1) substring of this string to a substring [pos2, pos2+count2) of sv
      REX_NO_DISCARD int32 compare(size_type pos1, size_type count1, const basic_string_view<value_type, traits_type> sv, size_type pos2, size_type count2 = s_npos) const
      {
        return internal::string_utils::compare<traits_type>(data() + pos1, sv.data() + pos2, count1, obj_length_or_count(sv, count2, pos2));
      }

      // checks if the string begins with the given prefix
      REX_NO_DISCARD bool starts_with(basic_string_view<value_type, traits_type> sv) const
      {
        return traits_type::compare(data(), sv.data(), sv.length()) == 0;
      }
      // checks if the string begins with the given prefix
      REX_NO_DISCARD bool starts_with(value_type c) const
      {
        return traits_type::eq(front(), c);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Nov/2022)
      // by adding a reference to the const_pointer overload, this one will only get selected if the argument
      REX_NO_DISCARD bool starts_with(const_pointer& s) const
      {
        return starts_with(rsl::string_view(s));
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // because we have the above function, we don't define the following
      // bool starts_with(const_pointer s) const;

      // checks if the string ends with the given suffix
      REX_NO_DISCARD bool ends_with(basic_string_view<value_type, traits_type> sv) const
      {
        return traits_type::compare(data() + (size() - sv.length()), sv.data(), sv.length()) == 0;
      }
      // checks if the string ends with the given suffix
      REX_NO_DISCARD bool ends_with(value_type c) const
      {
        return traits_type::eq(back(), c);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Nov/2022)
      // by adding a reference to the const_pointer overload, this one will only get selected if the argument
      REX_NO_DISCARD bool ends_with(const_pointer& s) const
      {
        return ends_with(rsl::string_view(s));
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // because we have the above function, we don't define the following
      // bool ends_with(const_pointer s) const;

      // checks if the string contains the given substring
      REX_NO_DISCARD bool contains(const basic_string_view<value_type, traits_type> sv) const
      {
        return find(sv) != s_npos;
      }
      // checks if the string contains the given substring
      REX_NO_DISCARD bool contains(value_type c) const
      {
        return find(c) != s_npos;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Nov/2022)
      // by adding a reference to the const_pointer overload, this one will only get selected if the argument
      REX_NO_DISCARD bool contains(const_pointer& s) const
      {
        return contains(rsl::string_view(s));
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // because we have the above function, we don't define the following
      // bool contains(const_pointer Size) const;

      // replaces the part of the string, indicated by [pos, pos + count) with a new string.
      basic_string& replace(size_type pos, size_type count, const basic_string& str)
      {
        REX_ASSERT_X(pos >= 0 && pos < length(), "pos out of range");

        count = (rsl::min)(count, size() - pos);
        return replace(pos, count, str.data(), str.length());
      }
      // replaces the part of the string, indicated by [first, last) with a new string.
      basic_string& replace(const_iterator first, const_iterator last, const basic_string& str)
      {
        const size_type first_idx = rsl::distance(cbegin(), first);
        const size_type last_idx  = rsl::distance(cbegin(), last);

        REX_ASSERT_X(first_idx < size(), "'first' iterator out of range");
        REX_ASSERT_X(last_idx <= size(), "'last' iterator out of range");

        return replace(first_idx, last_idx - first_idx, str.data(), str.length());
      }
      // replaces the part of the string, indicated by [pos, pos + count) with a new string [pos2, pos2 + count2).
      basic_string& replace(size_type pos1, size_type count1, const basic_string& str, size_type pos2, size_type count2)
      {
        count1 = (rsl::min)(count1, size() - pos1);
        count2 = (rsl::min)(count2, str.size() - pos2);

        REX_ASSERT_X(pos1 >= 0 && pos1 < length(), "pos1 out of range");
        REX_ASSERT_X(pos2 >= 0 && pos2 < str.length(), "pos2 out of range");

        return replace(pos1, count1, str.data() + pos2, count2);
      }
      // replaces the part of the string, indicated by [first, last) with a new string [first2, last2).
      template <typename InputIt>
      basic_string& replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2)
      {
        const size_type first_idx = rsl::distance(cbegin(), first);
        const size_type last_idx  = rsl::distance(cbegin(), last);

        REX_ASSERT_X(first_idx < size(), "'first' iterator out of range");
        REX_ASSERT_X(last_idx <= size(), "'last' iterator out of range");

        const_pointer s        = rsl::iterator_to_pointer(first2);
        const size_type length = static_cast<size_type>(rsl::distance(first2, last2));
        return replace(first_idx, last_idx - first_idx, s, length);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (01/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // replaces the part of the string, indicated by [pos, pos + count) with a new string literal.
      template <count_t Size>
      basic_string& replace(size_type pos, size_type count, const value_type (&s)[Size]) // NOLINT(modernize-avoid-c-arrays)
      {
        REX_ASSERT_X(pos >= 0 && pos < length(), "pos out of range");

        count = (rsl::min)(count, size() - pos);

        return replace(pos, count, s, Size - 1);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // because we define the above function, we don't define the following
      // basic_string& replace(size_type pos, size_type count, const_pointer s);
      basic_string& replace(size_type pos, size_type count, const_pointer s, size_type count2)
      {
        REX_ASSERT_X(pos >= 0 && pos < length(), "pos out of range");

        const size_type num_chars_to_add = count2 - count;
        const size_type old_size         = size();

        // can't do this if we're shrinking because
        // we might drop data that we need in the second copy phase
        if(num_chars_to_add > 0)
        {
          resize(size() + num_chars_to_add);
        }

        // copy the elements substring after pos + count to where they will be after replacement
        const size_type num_chars_after_copy = length() - (pos + count);
        const_pointer src                    = &m_begin[pos] + count;
        pointer dst                          = &m_begin[pos] + count2;
        traits_type::move(dst, src, num_chars_after_copy);

        // now copy the replacement string into the current string
        src = s;
        dst = &m_begin[pos];
        traits_type::move(dst, src, count2);

        m_end = m_begin + old_size + num_chars_to_add;
        traits_type::assign(*m_end, value_type());
        return *this;
      }

      /// RSL Comment: Not in ISO C++ Standard at time of writing (01/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // replaces the part of the string, indicated by [first, last) with a new string literal.
      template <count_t Size>
      basic_string& replace(const_iterator first, const_iterator last, const value_type (&s)[Size]) // NOLINT(modernize-avoid-c-arrays)
      {
        const size_type first_idx = rsl::distance(cbegin(), first);
        const size_type last_idx  = rsl::distance(cbegin(), last);

        REX_ASSERT_X(first_idx < size(), "'first' iterator out of range");
        REX_ASSERT_X(last_idx <= size(), "'last' iterator out of range");

        return replace(first_idx, last_idx - first_idx, s, Size - 1);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // because we define the above function, we don't define the following
      // basic_string& replace(const_iterator first, const_iterator last, const_pointer s);
      // basic_string& replace(const_iterator first, const_iterator last, const_pointer s, size_type count);

      // replaces the part of the string, indicated by [pos, pos + count) with count2 copies of ch
      basic_string& replace(size_type pos, size_type count, size_type count2, value_type ch)
      {
        REX_ASSERT_X(pos >= 0 && pos < length(), "pos out of range");

        count = (rsl::min)(count, size() - pos);
        if(count2 > count)
        {
          prepare_for_new_insert(pos + count, count2 - count);
        }

        traits_type::assign(rsl::addressof(m_begin[pos]), count2, ch);

        return *this;
      }
      // replaces the part of the string, indicated by [first, last) with count2 copies of ch
      basic_string& replace(const_iterator first, const_iterator last, size_type count2, value_type ch)
      {
        const size_type first_idx = rsl::distance(cbegin(), first);
        const size_type last_idx  = rsl::distance(cbegin(), last);

        REX_ASSERT_X(first_idx < size(), "'first' iterator out of range");
        REX_ASSERT_X(last_idx <= size(), "'last' iterator out of range");

        return replace(first_idx, last_idx - first_idx, count2, ch);
      }
      // replaces the part of the string, indicated by [first, last) with the characters in the initializer_list
      basic_string& replace(const_iterator first, const_iterator last, rsl::initializer_list<value_type> ilist)
      {
        return replace(first, last, ilist.begin(), ilist.end());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // replaces the part of the string, indicated by [pos, pos + count) with the characters of a string view
      basic_string& replace(size_type pos, size_type count, const basic_string_view<value_type, traits_type>& sv)
      {
        count = (rsl::min)(count, size() - pos);
        return replace(pos, count, sv.data(), sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // replaces the part of the string, indicated by [first, last) with the characters of a string view
      basic_string& replace(const_iterator first, const_iterator last, const basic_string_view<value_type, traits_type>& sv)
      {
        const size_type first_idx = rsl::distance(cbegin(), first);
        const size_type last_idx  = rsl::distance(cbegin(), last);

        REX_ASSERT_X(first_idx < size(), "'first' iterator out of range");
        REX_ASSERT_X(last_idx <= size(), "'last' iterator out of range");

        return replace(first_idx, last_idx - first_idx, sv.data(), sv.length());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // replaces the part of the string, indicated by [pos, pos + count) with the characters of a string view [pos2, pos2 + count2)
      basic_string& replace(size_type pos, size_type count, const basic_string_view<value_type, traits_type>& sv, size_type pos2, size_type count2 = s_npos)
      {
        count  = (rsl::min)(count, size() - pos);
        count2 = obj_length_or_count(sv, count2, pos2);
        return replace(pos, count, sv.data() + pos2, count2);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // Instead of returning a basic_string, this function returns a basic_string_view
      // returns a substring [pos, pos + count).
      basic_string_view<value_type, traits_type> substr(size_type pos = 0, size_type count = s_npos) const
      {
        count = obj_length_or_count(*this, count, pos);
        return basic_string_view<value_type, traits_type>(data() + pos, count);
      }

      // Copies a substring [pos, pos+count) to character string pointed to by dest.
      size_type copy(value_type* dst, size_type count, size_type pos = 0) const
      {
        count = (rsl::min)(count, size() - pos);
        traits_type::copy(dst, rsl::addressof(m_begin[pos]), count);
        return count;
      }

      // Resizes the string to contain count characters, fills the buffer with '\0'
      void resize(size_type count)
      {
        resize(count, value_type());
      }
      // Resizes the string to contain count characters, fills the buffer with ch
      void resize(size_type count, value_type ch)
      {
        if(count > size())
        {
          increase_capacity_if_needed(count - size());
          traits_type::assign(m_begin + size(), count - size(), ch);
        }

        m_end = m_begin + count;
        traits_type::assign(*m_end, value_type());
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // C++23 specifies a new function resize_and_overwrite, but I haven't implemented it yet.
      // check https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1072r10.html for more info.
      // template <typename Operation>
      // void resize_and_overwrite(size_type count, Operation op);

      // swaps the contents of the string with those of other
      void swap(basic_string& other)
      {
        // if both strings are using the sso buffer
        // then swap the contents, reassign the pointers
        if(is_using_sso_string() && other.is_using_sso_string())
        {
          rsl::swap(m_sso_buffer, other.m_sso_buffer);
          this->reset(m_sso_buffer.data(), m_sso_buffer.length(), m_sso_buffer.max_size());
          other.reset(other.m_sso_buffer.data(), other.m_sso_buffer.length(), other.m_sso_buffer.max_size());
          return;
        }
        // one is using heap memory, one is using the sso buffer
        // allocate heap memory for the one using the sso buffer
        // then swap the pointer
        else if(is_using_big_string() != other.is_using_big_string())
        {
          const size_type difference_in_size = rsl::abs(length() - other.length());
          if(is_using_sso_string())
          {
            reallocate(new_buffer_size(difference_in_size), data(), size());
          }
          else
          {
            other.reallocate(other.new_buffer_size(difference_in_size), other.data(), other.size());
          }
        }

        // this branch also gets executed if both strings use heap memory
        // which is exactly what we want
        rsl::swap(m_begin, other.m_begin);
        rsl::swap(m_end, other.m_end);
        rsl::swap(last(), other.last());
      }

      //
      // Searching
      //

      // finds the first substring equal to str
      REX_NO_DISCARD size_type find(const basic_string& str, size_type pos = 0) const
      {
        return internal::string_utils::find<traits_type, const_pointer>(m_begin, length(), pos, str.data(), str.length(), s_npos);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (03/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // finds the first substring equal to s
      template <count_t Size>
      REX_NO_DISCARD size_type find(const value_type (&s)[Size], size_type pos = 0) const // NOLINT(modernize-avoid-c-arrays)
      {
        return internal::string_utils::find<traits_type, const_pointer>(m_begin, length(), pos, s, Size - 1, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // because we define the above function, we don't define the following
      // size_type find(const_pointer s, size_type pos, size_type count) const;
      // size_type find(const_pointer s, size_type pos = 0) const;

      // finds the first character ch (treated as a single-character substring)
      REX_NO_DISCARD size_type find(value_type ch, size_type pos = 0) const
      {
        return internal::string_utils::find<traits_type, const_pointer>(m_begin, length(), pos, rsl::addressof(ch), 1_elem, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // find the first substring equal to sv
      REX_NO_DISCARD size_type find(const basic_string_view<value_type, traits_type>& sv, size_type pos = 0) const
      {
        return internal::string_utils::find<traits_type, const_pointer>(m_begin, length(), pos, sv.data(), sv.length(), s_npos);
      }

      // finds the last substring equal to str
      REX_NO_DISCARD size_type rfind(const basic_string& str, size_type pos = s_npos) const
      {
        if(pos == s_npos)
        {
          pos = length();
        }
        return internal::string_utils::rfind<traits_type, const_pointer>(m_begin, length(), pos, str.data(), str.length(), s_npos);
      }

      /// RSL Comment: Not in ISO C++ Standard at time of writing (03/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // finds the last substring equal to s
      template <count_t Size>
      REX_NO_DISCARD size_type rfind(const value_type (&s)[Size], size_type pos, size_type count) const // NOLINT(modernize-avoid-c-arrays)
      {
        return internal::string_utils::rfind<traits_type, const_pointer>(m_begin, length(), pos, s, Size - 1, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // because we define the above function, we don't define the following
      // size_type rfind(const_pointer str, size_type pos, size_type count) const;
      // size_type rfind(const_pointer str, size_type pos = s_npos) const;

      // finds the last character equal to ch
      REX_NO_DISCARD size_type rfind(value_type ch, size_type pos = s_npos) const
      {
        if(pos == s_npos)
        {
          pos = length();
        }
        return internal::string_utils::rfind<traits_type, const_pointer>(m_begin, length(), pos, rsl::addressof(ch), 1_elem, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // find the last substring equal to sv
      REX_NO_DISCARD size_type rfind(const basic_string_view<value_type, traits_type>& sv, size_type pos = s_npos) const
      {
        if(pos == s_npos)
        {
          pos = length();
        }
        return internal::string_utils::rfind<traits_type, const_pointer>(m_begin, length(), pos, sv.data(), sv.length(), s_npos);
      }

      // finds the first character equal to one of the characters in str
      REX_NO_DISCARD size_type find_first_of(const basic_string& str, size_type pos = 0) const
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_begin, length(), pos, str.data(), str.length(), s_npos);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (03/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // finds the first character equal to one of the characters in the string literal
      template <count_t Size>
      REX_NO_DISCARD size_type find_first_of(const value_type (&s)[Size], size_type pos) const // NOLINT(modernize-avoid-c-arrays)
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_begin, length(), pos, s, Size - 1, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // because we define the function above, we don't specify the following
      // size_type find_first_of(const_pointer s, size_type pos, size_type count) const;
      // size_type find_first_of(const_pointer s, size_type pos = 0) const;

      // finds the first character equal to ch
      REX_NO_DISCARD size_type find_first_of(value_type ch, size_type pos = 0) const
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_begin, length(), pos, rsl::addressof(ch), 1_elem, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // finds the first character equal to a character in sv
      REX_NO_DISCARD size_type find_first_of(const basic_string_view<value_type, traits_type>& sv, size_type pos = 0) const
      {
        return internal::string_utils::find_first_of<traits_type, const_pointer>(m_begin, length(), pos, sv.data(), sv.length(), s_npos);
      }

      // finds the first character equal to none of the characters in str
      REX_NO_DISCARD size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_begin, length(), pos, str.data(), str.length(), s_npos);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (03/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // finds the first character equal to none of the characters in the string literal
      template <count_t Size>
      REX_NO_DISCARD size_type find_first_not_of(const value_type (&s)[Size], size_type pos = 0) const // NOLINT(modernize-avoid-c-arrays)
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_begin, length(), pos, s, Size - 1, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // because we define the above function, we don't define the following
      // size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const;
      // size_type find_first_not_of(const_pointer s, size_type pos = s_npos) const;

      // finds the first character not equal to ch
      REX_NO_DISCARD size_type find_first_not_of(value_type ch, size_type pos = 0) const
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_begin, length(), pos, rsl::addressof(ch), 1_elem, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // finds the first character not equal to any character in sv
      REX_NO_DISCARD size_type find_first_not_of(const basic_string_view<value_type, traits_type>& sv, size_type pos = 0) const
      {
        return internal::string_utils::find_first_not_of<traits_type, const_pointer>(m_begin, length(), pos, sv.data(), sv.length(), s_npos);
      }

      // finds the last character equal to one of the characters in str
      REX_NO_DISCARD size_type find_last_of(const basic_string& str, size_type pos = s_npos) const
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_begin, length(), pos, str.data(), str.length(), s_npos);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (03/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // finds the last character equal to one of the characters in the string literal
      template <count_t Size>
      REX_NO_DISCARD size_type find_last_of(const value_type (&s)[Size], size_type pos) const // NOLINT(modernize-avoid-c-arrays)
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_begin, length(), pos, s, Size - 1, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // because we define the above function, we don't define the following
      // size_type find_last_of(const value_type* s, size_type pos, size_type count) const;
      // size_type find_last_of(const value_type* s, size_type pos = s_npos) const;

      // finds the last character equal to ch
      REX_NO_DISCARD size_type find_last_of(value_type ch, size_type pos = s_npos) const
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_begin, length(), pos, rsl::addressof(ch), 1_elem, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // finds the last character equal to a character in sv
      REX_NO_DISCARD size_type find_last_of(const basic_string_view<value_type, traits_type>& sv, size_type pos = s_npos) const
      {
        return internal::string_utils::find_last_of<traits_type, const_pointer>(m_begin, length(), pos, sv.data(), sv.length(), s_npos);
      }

      // finds the last character equal to none of the characters in str
      REX_NO_DISCARD size_type find_last_not_of(const basic_string& str, size_type pos = s_npos) const
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_begin, length(), pos, str.data(), str.length(), s_npos);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (03/Jul/2022)
      // The standard doesn't provide an overload for a string literal
      // finds the last character equal to one of the characters in the string literal
      template <count_t Size>
      REX_NO_DISCARD size_type find_last_not_of(const value_type (&s)[Size], size_type pos) const // NOLINT(modernize-avoid-c-arrays)
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_begin, length(), pos, s, Size - 1, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // because we define the above function, we don't define the following
      // size_type find_last_not_of(const_pointer s, size_type pos, size_type count) const;
      // size_type find_last_not_of(const_pointer s, size_type pos = s_npos) const;

      // finds the last character not equal to ch
      REX_NO_DISCARD size_type find_last_not_of(value_type ch, size_type pos = s_npos) const
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_begin, length(), pos, rsl::addressof(ch), 1_elem, s_npos);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (01/Jul/2022)
      // This is a template function in the standard due to ambiguous overload
      // as both basic_string and basic_string_view<value_type, traits_type> can be implicitly created with a const char*.
      // This is not possible in RSL though as the ctor for const char* for string is explicit.
      // Therefore this takes a basic_string_view
      // finds the last character equal to none of the characters in sv
      REX_NO_DISCARD size_type find_last_not_of(const basic_string_view<value_type, traits_type>& sv, size_type pos = s_npos) const
      {
        return internal::string_utils::find_last_not_of<traits_type, const_pointer>(m_begin, length(), pos, sv.data(), sv.length(), s_npos);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
      // npos is a function in RSL, not access to a public static variable
      // special value, the meaning is different depending on the context.
      // usually it means to indicate that something is not found.
      static size_type npos()
      {
        return s_npos;
      }

      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to an int32. same as rsl::stoi
      REX_NO_DISCARD rsl::optional<int32> to_int(int32 base = 10) const
      {
        return rsl::strtoi(data(), nullptr, base);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to a long. same as rsl::stol
      REX_NO_DISCARD rsl::optional<long> to_long(int32 base = 10) const
      {
        return rsl::strtol(data(), nullptr, base);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to a long long. same as rsl::stoll
      REX_NO_DISCARD rsl::optional<int64> to_long64(int32 base = 10) const
      {
        return rsl::strtoll(data(), nullptr, base);
      }

      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to an unsigned int32. same as rsl::stoui
      REX_NO_DISCARD rsl::optional<uint32> to_uint(int32 base = 10) const
      {
        return rsl::strtoui(data(), nullptr, base);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to an unsigned long. same as rsl::stoul
      REX_NO_DISCARD rsl::optional<ulong> to_ulong(int32 base = 10) const
      {
        return rsl::strtoul(data(), nullptr, base);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to an unsigned long long. same as rsl::stoull
      REX_NO_DISCARD rsl::optional<uint64> to_ulong64(int32 base = 10) const
      {
        return rsl::strtoull(data(), nullptr, base);
      }

      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to a float. same as rsl::stof
      REX_NO_DISCARD rsl::optional<float32> to_float(int32 /*base*/ = 10) const
      {
        return rsl::strtof(data(), nullptr);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to a double. same as rsl::stod
      REX_NO_DISCARD rsl::optional<float64> to_double(int32 /*base*/ = 10) const
      {
        return rsl::strtod(data(), nullptr);
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
      // converts the string to a long double. same as rsl::stold
      REX_NO_DISCARD rsl::optional<lfloat64> to_lfloat(int32 /*base*/ = 10) const
      {
        return rsl::strtold(data(), nullptr);
      }

    private:
      // resets the string so m_begin, m_end and m_last
      // point to its sso buffer
      void reset()
      {
        REX_ASSERT_X(is_using_big_string(), "Resetting string that's using heap memory, this will cause a memory leak");

        reset(m_sso_buffer.data(), 0, m_sso_buffer.max_size());
      }
      // sets the data pointers of the string
      void reset(pointer buffer, size_type length, size_type bufferSize)
      {
        m_begin = buffer;
        m_end   = m_begin + length;
        last()  = m_begin + bufferSize;
      }
      // assigns a string into the sso buffer,
      // setting m_begin, m_end and m_last
      void sso_assign(const_pointer str, size_type length)
      {
        REX_ASSERT_X(is_using_sso_string(), "Not using sso string when using sso assign");
        REX_ASSERT_X(str[length] == value_type(), "str doesn't end in a null termination char");

        m_sso_buffer.assign(str, length);
        m_end = m_sso_buffer.data() + length;
      }
      // assigns a string into the heap buffer
      // setting m_begin, m_end and m_last
      // this function should not be called when using the sso buffer
      // length doesn't include the null termination char
      void heap_assign(const_pointer str, size_type length)
      {
        REX_ASSERT_X(is_using_big_string(), "Not using big string when using heap assign");
        REX_ASSERT_X(str[length] == value_type(), "str doesn't end in a null termination char");

        if(length > capacity())
        {
          // we shouldn't call reserve here as that'll copy the current data over to the new buffer
          // this is pointless as we want to overwrite the data anyway.
          reallocate(length + 1, str, length);
        }
      }
      // assigns a rvalue str to this.
      // copies when using small string, swaps when using big string
      void move_assign(rsl::basic_string<CharType>&& str)
      {
        if(str.is_using_big_string())
        {
          // if the other string is using heap memory,
          // we can just copy over the pointers and reset the source string.
          m_begin = str.m_begin;
          m_end   = str.m_end;
          last()  = str.last();

          str.reset();
        }
        else
        {
          resize(str.length());
          rsl::memcpy(data(), str.data(), str.length());
        }
      }
      // allocates a new buffer and copies over the data
      // begin_src should point to a null terminated char
      // setting m_begin, m_end and m_last
      void reallocate(size_type newCapacity, const_pointer beginSrc, size_type length)
      {
        pointer new_buffer = static_cast<pointer>(get_allocator().allocate(newCapacity));

        traits_type::copy(new_buffer, beginSrc, length);
        deallocate();

        m_begin = new_buffer;
        m_end   = m_begin + length;
        traits_type::assign(*m_end, value_type());
        last() = m_begin + newCapacity;
      }
      // deallocates all heap data, if used.
      void deallocate()
      {
        if(is_using_big_string())
        {
          get_allocator().deallocate(data());
        }
      }
      // moves every element starting at str[idx] 'count' space(s) to the right
      void prepare_for_new_insert(size_type idx, size_type count = 1)
      {
        if(size() + count >= capacity())
        {
          // if we have to reallocate, we want to copy over elements in the range [0, idx)
          // and then copy the remaining elements in [idx, end())
          // then release the old buffer.
          const size_type size_for_new_buffer = new_buffer_size(count);
          pointer new_buffer                  = static_cast<pointer>(get_allocator().allocate(calc_bytes_needed(size_for_new_buffer)));

          // we now have 2 buffers, 1 buffer holding our data, and 1 that is prepared to receive a copy of this data
          // +---+---+---+---+---+---+---+---+---+---+---+---+
          // | h | e | l | l | o |   | w | o | r | l | d | \0|
          // +---+---+---+---+---+---+---+---+---+---+---+---+
          // eg. Word to insert: "new " before 'w' of "world"
          // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
          // | x | x | x | x | x | x | x | x | x | x | x | x | x | x | x | x |
          // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

          rsl::memcpy(new_buffer, data(), calc_bytes_needed(idx));
          pointer dst_into_new_buffer = new_buffer + idx + count;
          rsl::memcpy(dst_into_new_buffer, data() + idx, calc_bytes_needed(length() - idx));

          // we now have 2 buffers, 1 buffer holding our data, and 1 that holds a copy of our original string.
          // However, there is memory left uninitialized to prepare for insertion of a new string
          // +---+---+---+---+---+---+---+---+---+---+---+---+
          // | h | e | l | l | o |   | w | o | r | l | d | \0|
          // +---+---+---+---+---+---+---+---+---+---+---+---+
          // eg. Word to insert: "new " before 'w' of "world"
          // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
          // | h | e | l | l | o |   | x | x | x | x | w | o | r | l | d | \0|
          // +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

          // all that's left is to deallocate the old buffer and reset the data pointers.
          deallocate();

          const size_type new_size = size() + count;
          m_begin                  = new_buffer;
          m_end                    = m_begin + new_size;
          last()                   = m_begin + size_for_new_buffer;
        }
        else
        {
          // using move here as the dst and src could overlap
          pointer dst_in_buffer = data() + idx + count;
          traits_type::move(dst_in_buffer, data() + idx, calc_bytes_needed(length() - idx));
          m_end += count;
        }

        traits_type::assign(*m_end, value_type());
      }
      // moves every element starting at 'pos' 'count' space(s) to the right
      void prepare_for_new_insert(const_iterator pos, size_type count = 1)
      {
        const auto pos_idx = rsl::distance(cbegin(), pos);
        prepare_for_new_insert(pos_idx, count);
      }
      // reallocates if new size is bigger than the sso and bigger than the current capacity
      bool increase_capacity_if_needed(size_type numElementsToAdd)
      {
        if(size() + numElementsToAdd > capacity())
        {
          reallocate(new_buffer_size(numElementsToAdd), data(), size());
          return true;
        }

        return false;
      }
      // checks if the string is using heap memory
      bool is_using_big_string() const
      {
        return m_begin != m_sso_buffer.data();
      }
      // checks if the string is using the sso
      bool is_using_sso_string() const
      {
        return !is_using_big_string();
      }
      // returns the size of the string, in bytes
      count_t size_in_bytes() const
      {
        return calc_bytes_needed(size());
      }
      // returns the capacity of the string, in bytes
      count_t capacity_in_bytes() const
      {
        return calc_bytes_needed(capacity());
      }
      // calculates the bytes needed for the length provided
      count_t calc_bytes_needed(count_t length) const
      {
        return sizeof(value_type) * length;
      }
      // returns the minimum between either the length of sv or count
      size_type obj_length_or_count(const basic_string_view<value_type, traits_type>& sv, size_type count, size_type startPos = 0) const
      {
        return (count == s_npos || count > sv.length()) ? sv.length() - startPos : count;
      }
      // returns the minimum between either the length of s or count
      size_type obj_length_or_count(const basic_string& s, size_type count, size_type startPos = 0)
      {
        return (count == s_npos || count > s.length()) ? s.length() - startPos : count;
      }
      // returns the size of a new buffer on reallocation
      size_type new_buffer_size(size_type numElementsToAdd) const
      {
        return size() * 2 + numElementsToAdd + 1;
      }
      // replace substring with given substring, doesn't perform any checking
      basic_string& replace(size_type pos, size_type count1, pointer str, size_type count2)
      {
        if(count2 == count1)
        {
          traits_type::copy(rsl::addressof(m_begin[pos]), str, count2);
        }
        else if(count2 > count1)
        {
          prepare_for_new_insert(pos + count1, count2 - count1);
          traits_type::copy(rsl::addressof(m_begin[pos]), str, count2);
        }
        else
        {
          traits_type::copy(rsl::addressof(m_begin[pos]), str, str);
          erase(pos + count2, count1 - count2);
        }

        return *this;
      }
      // returns the allocator used by the string
      allocator_type& get_allocator()
      {
        return m_cp_last_and_allocator.second();
      }

    private:
      pointer& last()
      {
        return m_cp_last_and_allocator.first();
      }
      const_pointer last() const
      {
        return m_cp_last_and_allocator.first();
      }

      rsl::stack_string<CharType, s_sso_buff_size> m_sso_buffer;
      pointer m_begin;
      pointer m_end;
      rsl::compressed_pair<pointer, allocator_type> m_cp_last_and_allocator;
    };

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(const rsl::basic_string<Char, Traits, Alloc>& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str;
      str += lhs;
      str += rhs;
      return str;
    }

    /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
    // the standard doesn't provide an overload for a string literal.
    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    rsl::basic_string<Char, Traits, Alloc> operator+(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      rsl::basic_string<Char, Traits, Alloc> str;
      str += lhs;
      str += rhs;
      return str;
    }
    /// RSL Comment: Different from ISO C++ Standard at time of writing (07/Jul/2022)
    // because we define the function above, we don't define the following
    // template <typename Char, typename Traits, typename Alloc, count_t Size>
    // rsl::basic_string<Char, Traits, Alloc> operator+(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&s)[Size]);

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(const rsl::basic_string<Char, Traits, Alloc>& lhs, Char rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str;
      str += lhs;
      str += rhs;
      return str;
    }

    /// RSL Comment: Not in ISO C++ Standard at time of writing (07/Jul/2022)
    // the standard doesn't provide an overload for a string literal.
    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    rsl::basic_string<Char, Traits, Alloc> operator+(const Char (&lhs)[Size], const rsl::basic_string<Char, Traits, Alloc>& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      rsl::basic_string<Char, Traits, Alloc> str;
      str += lhs;
      str += rhs;
      return str;
    }
    /// RSL Comment: Different from ISO C++ Standard at time of writing (07/Jul/2022)
    // because we define the function above, we don't define the following
    // template <typename Char, typename Traits, typename Alloc>
    // rsl::basic_string<Char, Traits, Alloc> operator+(const Char* s, const rsl::basic_string<Char, Traits, Alloc>& rhs);

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(Char lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str;
      str += lhs;
      str += rhs;
      return str;
    }

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(rsl::basic_string<Char, Traits, Alloc>&& lhs, rsl::basic_string<Char, Traits, Alloc>&& rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str(rsl::move(lhs));
      str += rhs;
      return str;
    }

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(rsl::basic_string<Char, Traits, Alloc>&& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str(rsl::move(lhs));
      str += rhs;
      return str;
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Jul/2022)
    // Creating the following overload instead of just using Char* to make it more performant
    // for string literals as we don't have to calculate its length
    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    rsl::basic_string<Char, Traits, Alloc> operator+(rsl::basic_string<Char, Traits, Alloc>&& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      rsl::basic_string<Char, Traits, Alloc> str(rsl::move(lhs));
      str += rhs;
      return str;
    }

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(const rsl::basic_string<Char, Traits, Alloc>&& lhs, Char rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str(rsl::move(lhs));
      str += rhs;
      return str;
    }

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(const rsl::basic_string<Char, Traits, Alloc>& lhs, rsl::basic_string<Char, Traits, Alloc>&& rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str(rsl::move(rhs));
      str.insert(str.cbegin(), lhs);
      return str;
    }

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    rsl::basic_string<Char, Traits, Alloc> operator+(const Char (&lhs)[Size], rsl::basic_string<Char, Traits, Alloc>&& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      rsl::basic_string<Char, Traits, Alloc> str(rsl::move(rhs));
      str.insert(str.cbegin(), lhs);
      return str;
    }

    // returns a string containing characters from lhs followed by the characters from rhs
    template <typename Char, typename Traits, typename Alloc>
    rsl::basic_string<Char, Traits, Alloc> operator+(Char lhs, rsl::basic_string<Char, Traits, Alloc>&& rhs)
    {
      rsl::basic_string<Char, Traits, Alloc> str(rsl::move(rhs));
      str.insert(str.cbegin(), lhs);
      return str;
    }

    // compares if 2 string objects are equal
    template <typename Char, typename Traits, typename Alloc>
    bool operator==(const rsl::basic_string<Char, Traits, Alloc>& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      if(lhs.length() != rhs.length())
      {
        return false;
      }

      return Traits::compare(lhs.data(), rhs.data(), lhs.length()) == 0;
    }
    // compares if 2 string objects are not equal
    template <typename Char, typename Traits, typename Alloc>
    bool operator!=(const rsl::basic_string<Char, Traits, Alloc>& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      return !(lhs == rhs);
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc>
    bool operator<(const rsl::basic_string<Char, Traits, Alloc>& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      return lhs.compare(rhs) < 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc>
    bool operator<=(const rsl::basic_string<Char, Traits, Alloc>& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      return lhs.compare(rhs) <= 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc>
    bool operator>(const rsl::basic_string<Char, Traits, Alloc>& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      return lhs.compare(rhs) > 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc>
    bool operator>=(const rsl::basic_string<Char, Traits, Alloc>& lhs, const rsl::basic_string<Char, Traits, Alloc>& rhs)
    {
      return lhs.compare(rhs) >= 0;
    }
    // compares if 2 strings are equal
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator==(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      if(lhs.length() != Size - 1)
      {
        return false;
      }

      return Traits::compare(lhs.data(), rhs, lhs.length()) == 0;
    }
    // compares if 2 strings are equal
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator==(const Char (&lhs)[Size], const rsl::basic_string<Char, Traits, Alloc>& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      return rhs == lhs;
    }
    // compares if 2 strings are not equal
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator!=(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return !(lhs == rhs);
    }
    // compares if 2 strings are not equal
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator!=(const Char (&lhs)[Size], const rsl::basic_string<Char, Traits, Alloc>& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      return !(rhs == lhs);
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator<(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) < 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator<(const Char (&lhs)[Size], const rsl::basic_string<Char, Traits, Alloc>& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      return rhs.compare(lhs) > 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator<=(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) <= 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator<=(const Char (&lhs)[Size], const rsl::basic_string<Char, Traits, Alloc>& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      return rhs.compare(lhs) >= 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator>(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) > 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator>(const Char (&lhs)[Size], const rsl::basic_string<Char, Traits, Alloc>& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      return rhs.compare(lhs) < 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator>=(const rsl::basic_string<Char, Traits, Alloc>& lhs, const Char (&rhs)[Size]) // NOLINT(modernize-avoid-c-arrays)
    {
      return lhs.compare(rhs) >= 0;
    }
    // lexicographically compares 2 strings
    template <typename Char, typename Traits, typename Alloc, count_t Size>
    bool operator>=(const Char (&lhs)[Size], const rsl::basic_string<Char, Traits, Alloc>& rhs) // NOLINT(modernize-avoid-c-arrays)
    {
      return rhs.compare(lhs) <= 0;
    }

    // swaps the contents of lhs and rhs
    template <typename Char, typename Traits, typename Alloc>
    void swap(basic_string<Char, Traits, Alloc>& lhs, basic_string<Char, Traits, Alloc>& rhs)
    {
      lhs.swap(rhs);
    }

    // erases all elements that compare equal to the value from the container
    template <typename Char, typename Traits, typename Alloc, typename U>
    typename basic_string<Char, Traits, Alloc>::size_type erase(basic_string<Char, Traits, Alloc>& c, const U& value)
    {
      auto it = rsl::remove(c.begin(), c.end(), value);
      auto r  = rsl::distance(it, c.end());
      c.erase(it, c.end());
      return r;
    }
    // erases all elements that satisfy the predicate pred from the container
    template <typename Char, typename Traits, typename Alloc, typename Predicate>
    typename basic_string<Char, Traits, Alloc>::size_type erase_if(basic_string<Char, Traits, Alloc>& c, Predicate pred)
    {
      auto it = rsl::remove_if(c.begin(), c.end(), pred);
      auto r  = rsl::distance(it, c.end());
      c.erase(it, c.end());
      return r;
    }

    using string    = basic_string<char8>;
    using wstring   = basic_string<tchar>;
    using u16string = basic_string<char16_t>;
    using u32string = basic_string<char32_t>;

    // template <typename Char, typename Traits>
    // class basic_istream;

    // extracts characters from input and appends them to str until the delim is found or the stream's eof.
    template <typename Char, typename Traits, typename Allocator>
    basic_istream<Char, Traits>& getline(basic_istream<Char, Traits>& input, basic_string<Char, Traits, allocator>& str, Char delim)
    {
      str.erase();
      input.getline(str, delim);
      return input;
    }

    // extracts characters from input and appends them to str until the '\n' is found or the stream's eof.
    template <typename Char, typename Traits, typename Allocator>
    basic_istream<Char, Traits>& getline(basic_istream<Char, Traits>& input, basic_string<Char, Traits, allocator>& str)
    {
      return getline(input, str, '\n');
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // returns an optional instead of a basic type
    // interprets a signed integer value in the string
    rsl::optional<int32> stoi(const string& str, char8* strEnd = nullptr, int32 base = 10);
    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // returns an optional instead of a basic type
    rsl::optional<int32> stoi(const wstring& /*str*/, tchar* /*str_end*/ = nullptr, int32 /*base*/ = 10);
    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // returns an optional instead of a basic type
    // interprets a signed integer value in the string
    rsl::optional<long> stol(const string& str, char8* strEnd = nullptr, int32 base = 10);
    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // returns an optional instead of a basic type
    rsl::optional<long> stol(const wstring& /*str*/, tchar* /*str_end */ = nullptr, int32 /*base */ = 10);
    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // returns an optional instead of a basic type
    // interprets a signed integer value in the string
    rsl::optional<int64> stoll(const string& str, char8* strEnd = nullptr, int32 base = 10);
    /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)
    // returns an optional instead of a basic type
    rsl::optional<int64> stoll(const wstring& /*str*/, tchar* /*str_end */ = nullptr, int32 /*base*/ = 10);

    /// RSL Comment: Not in ISO C++ Standard at time of writing (04/Jul/2022)
    // For some reason, the standard doesn't provide a conversion to unsigned int32.
    // interprets an unsigned integer value in the string
    rsl::optional<uint32> stoui(const string& str, char8* strEnd = nullptr, int32 base = 10);
    rsl::optional<uint32> stoui(const wstring& /*str*/, tchar* /*str_end */ = nullptr, int32 /*base */ = 10);
    // interprets an unsigned integer value in the string
    rsl::optional<ulong> stoul(const string& str, char8* strEnd = nullptr, int32 base = 10);
    rsl::optional<ulong> stoul(const wstring& /*str*/, tchar* /*str_end*/ = nullptr, int32 /*base*/ = 10);
    // interprets an unsigned integer value in the string
    rsl::optional<uint64> stoull(const string& str, char8* strEnd = nullptr, int32 base = 10);
    rsl::optional<uint64> stoull(const wstring& /*str*/, tchar* /*str_end */ = nullptr, int32 /*base */ = 10);

    // interprets a floating point value in a string
    rsl::optional<float32> stof(const string& str, char8* strEnd = nullptr);
    rsl::optional<float32> stof(const wstring& /*str*/, tchar* /*str_end */ = nullptr);
    // interprets a floating point value in a string
    rsl::optional<float64> stod(const string& str, char8* strEnd = nullptr);
    rsl::optional<float64> stod(const wstring& /*str*/, tchar* /*str_end */ = nullptr);
    // interprets a floating point value in a string
    rsl::optional<lfloat64> stold(const string& str, char8* strEnd = nullptr);
    rsl::optional<lfloat64> stold(const wstring& /*str*/, tchar* /*str_end */ = nullptr);

    // converts a signed integer to a string
    string to_string(int32 value);
    // converts a signed integer to a string
    string to_string(long value);
    // converts a signed integer to a string
    string to_string(int64 value);
    // converts a unsigned integer to a string
    string to_string(uint32 value);
    // converts a unsigned integer to a string
    string to_string(ulong value);
    // converts a unsigned integer to a string
    string to_string(uint64 value);
    // converts a floating point to a string
    string to_string(float32 value);
    // converts a floating point to a string
    string to_string(float64 value);
    // converts a floating point to a string
    string to_string(lfloat64 value);

    // converts a signed integer to a wstring
    wstring to_wstring(int32 value);
    // converts a signed integer to a wstring
    wstring to_wstring(long value);
    // converts a signed integer to a wstring
    wstring to_wstring(int64 value);
    // converts a unsigned integer to a wstring
    wstring to_wstring(uint32 value);
    // converts a unsigned integer to a wstring
    wstring to_wstring(ulong value);
    // converts a unsigned integer to a wstring
    wstring to_wstring(uint64 value);
    // converts a floating point to a wstring
    wstring to_wstring(float32 value);
    // converts a floating point to a wstring
    wstring to_wstring(float64 value);
    // converts a floating point to a wstring
    wstring to_wstring(lfloat64 value);

    namespace string_literals
    {
#if defined(REX_MSVC_COMPILER)
  #pragma warning(push)
  #pragma warning(disable : 4455) // literal suffix identifiers that do not start with an underscore are reserved
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wuser-defined-literals" // literal suffix identifiers that do not start with an underscore are reserved
#endif
      // returns a string of the desired type
      string operator""s(const char8* s, size_t len); // NOLINT(clang-diagnostic-user-defined-literals)
      // returns a string of the desired type
      u16string operator""s(const char16_t* s, size_t len); // NOLINT(clang-diagnostic-user-defined-literals)
      // returns a string of the desired type
      u32string operator""s(const char32_t* s, size_t len); // NOLINT(clang-diagnostic-user-defined-literals)
      // returns a string of the desired type
      wstring operator""s(const tchar* s, size_t len); // NOLINT(clang-diagnostic-user-defined-literals)
#if defined(REX_MSVC_COMPILER)
  #pragma warning(pop)
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic pop
#endif
    } // namespace string_literals

    template <typename T>
    struct hash;

    template <typename CharType, typename Traits, typename Allocator>
    struct hash<basic_string<CharType, Traits, Allocator>>
    {
      hash_result operator()(const basic_string<CharType, Traits, allocator>& str) const
      {
        return rsl::internal::hash(str.data());
      }
    };

    template <typename InputIt, typename Alloc = rsl::allocator>
    basic_string(InputIt, InputIt, Alloc = Alloc()) -> basic_string<typename rsl::iterator_traits<InputIt>::value_type, rsl::char_traits<typename rsl::iterator_traits<InputIt>::value_type>, Alloc>;

    template <typename Char, typename Traits, typename Alloc = rsl::allocator>
    explicit basic_string(rsl::basic_string_view<Char, Traits>, const Alloc& = Alloc()) -> basic_string<Char, Traits, Alloc>;

    template <typename Char, typename Traits, typename Alloc = rsl::allocator>
    explicit basic_string(rsl::basic_string_view<Char, Traits>, typename rsl::basic_string_view<Char, Traits>::size_type, typename rsl::basic_string_view<Char, Traits>::size_type, const Alloc& = Alloc()) -> basic_string<Char, Traits, Alloc>;

    template <typename Char, typename Traits>
    class basic_ostream;

  } // namespace v1
} // namespace rsl

// stores each characters from the resulting sequence to the output stream
template <typename Char, typename Traits, typename Allocator>
inline rsl::basic_ostream<Char, Traits>& operator<<(rsl::basic_ostream<Char, Traits>& os, const rsl::basic_string<Char, Traits, Allocator>& str)
{
  os.rdbuf()->sputn(str.data(), str.length());
  return os;
}