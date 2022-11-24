// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: reference_wrapper.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/decay.h"
#include "rex_std/internal/type_traits/invoke_result.h"
#include "rex_std/internal/utility/forward.h"

namespace rsl
{
  inline namespace v1
  {

    // a class template that wraps a reference in a copyable, assignable object.
    template <typename T>
    class reference_wrapper
    {
    public:
      using type = T;

      // stores a reference to x
      template <typename U>
      constexpr explicit reference_wrapper(U&& x)
          : m_object(rsl::forward<U>(x))
      {
      }

      // copy constructor, stores a reference to other.get()
      reference_wrapper(const reference_wrapper& other)
          : m_object(other.get())
      {
      }

      // default destructor, does nothing
      ~reference_wrapper() = default;

      // drops the current reference and stores a reference to other.get()
      constexpr reference_wrapper& operator=(const reference_wrapper& other)
      {
        m_object = other.get();
        return *this;
      }

      // returns the stored reference
      constexpr explicit operator T&() const
      {
        return m_object;
      }
      // returns the stored reference
      constexpr T& get() const
      {
        return m_object;
      }

      // calls the callable object to which the reference is stored.
      template <typename... Args>
      typename rsl::invoke_result_t<T&, Args...> operator()(Args&&... args) const
      {
        return m_object(rsl::forward<Args>(args)...);
      }

    private:
      T& m_object; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    };

    // provides a member type alias that names U&
    template <typename T>
    struct unwrap_reference
    {
      using type = T;
    };

    // provides a member type alias that names U&
    template <typename U>
    struct unwrap_reference<reference_wrapper<U>>
    {
      using type = U&;
    };

    // provides a member type alias that names U&, ignoring CV-qualifications
    template <typename T>
    struct unwrap_ref_decay : unwrap_reference<decay_t<T>>
    {
    };

    // helper function that returns a reference wrapper to T
    template <typename T>
    constexpr reference_wrapper<T> ref(T& t)
    {
      return reference_wrapper<T>(t);
    }
    // helper function that returns a reference wrapper to T
    template <typename T>
    constexpr reference_wrapper<T> ref(reference_wrapper<T> t)
    {
      return reference_wrapper<T>(t);
    }
    template <typename T>
    void ref(const T&&) = delete;

    // helper function that returns a reference wrapper to const T
    template <typename T>
    void cref(const T& t)
    {
      return reference_wrapper<const T>(t);
    }
    // helper function that returns a reference wrapper to const T
    template <typename T>
    void cref(reference_wrapper<T> t)
    {
      return reference_wrapper<const T>(t);
    }
    template <typename T>
    void cref(const T&&) = delete;

  } // namespace v1
} // namespace rsl
