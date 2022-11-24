// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: optional.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/functional/hash_result.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/type_traits/aligned_storage.h"
#include "rex_std/internal/type_traits/conjunction.h"
#include "rex_std/internal/type_traits/decay.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_constructible.h"
#include "rex_std/internal/type_traits/is_reference.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/is_trivially_destructible.h"
#include "rex_std/internal/type_traits/negation.h"
#include "rex_std/internal/type_traits/remove_const.h"
#include "rex_std/internal/type_traits/remove_cvref.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/in_place.h"
#include "rex_std/internal/utility/move.h"
#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    // nullopt_t is a class type used indicate an empty optional
    struct nullopt_tag
    {
    };

    struct nullopt_t
    {
      constexpr nullopt_t(nullopt_tag /*unused*/ = {}) {} // NOLINT(google-explicit-constructor)
    };

    constexpr nullopt_t nullopt {nullopt_tag {}};

    namespace internal
    {
      template <typename T, bool IsTriviallyDestructible = is_trivially_destructible_v<T>>
      struct optional_storage
      {
      public:
        using value_type = rsl::remove_const_t<T>;

        constexpr optional_storage()
            : m_has_value(false)
            , m_val()
        {
        }

        constexpr explicit optional_storage(const value_type& v)
            : m_has_value(true)
        {
          new(rsl::addressof(m_val)) value_type(v);
        }

        constexpr explicit optional_storage(value_type&& v)
            : m_has_value(true)
        {
          new(rsl::addressof(m_val)) value_type(rsl::move(v));
        }

        template <typename... Args>
        constexpr explicit optional_storage(in_place_t /*unused*/, Args&&... args)
            : m_has_value(true)
        {
          new(rsl::addressof(m_val)) value_type(rsl::forward<Args>(args)...);
        }

        template <typename U, typename... Args, typename = enable_if_t<is_constructible_v<T, rsl::initializer_list<U>&, Args&&...>>>
        constexpr explicit optional_storage(in_place_t /*unused*/, rsl::initializer_list<U> ilist, Args&&... args)
            : m_has_value(true)
        {
          new(rsl::addressof(m_val)) value_type(ilist, rsl::forward<Args>(args)...);
        }

        constexpr optional_storage(const optional_storage&) = default;
        constexpr optional_storage(optional_storage&&)      = default;

        ~optional_storage()
        {
          if(m_has_value)
          {
            destroy_value();
          }
        }

        constexpr optional_storage& operator=(const optional_storage&) = default;
        constexpr optional_storage& operator=(optional_storage&&)      = default;

        template <typename... Args>
        constexpr void construct_value(Args&&... args)
        {
          new(m_val.get()) value_type(rsl::forward<Args>(args)...);
        }
        constexpr void destroy_value()
        {
          m_val.~value_type();
        }

        constexpr bool has_value() const
        {
          return m_has_value;
        }
        constexpr void has_value(bool newHasValue)
        {
          m_has_value = newHasValue;
        }
        constexpr void set_val(const value_type& val)
        {
          m_val = val;
        }
        constexpr void set_val(value_type&& val)
        {
          m_val = rsl::move(val);
        }
        constexpr const value_type& val() const
        {
          return *m_val.get();
        }
        constexpr value_type& val()
        {
          return *m_val.get();
        }

        constexpr void swap(const optional_storage<value_type>& other)
        {
          if(has_value() == other.has_value())
          {
            if(has_value())
            {
              rsl::swap(val(), other.val());
            }
          }
          else
          {
            if(has_value())
            {
              other.construct_value(rsl::move(val()));
              destroy_value();
            }
            else
            {
              construct_value(rsl::move(other.val()));
              other.destroy_value();
            }

            rsl::swap(m_has_value, other.m_has_value);
          }
        }

      private:
        bool m_has_value;
        aligned_storage<value_type> m_val {};
      };

      template <typename T>
      class optional_storage<T, true>
      {
      public:
        using value_type = rsl::remove_const_t<T>;

        constexpr optional_storage()
            : m_has_value(false)
        {
        }

        constexpr explicit optional_storage(const value_type& v)
            : m_has_value(true)
        {
          m_val.set(v);
        }

        constexpr explicit optional_storage(value_type&& v)
            : m_has_value(true)
        {
          m_val.set(rsl::move(v));
        }

        template <typename... Args>
        constexpr explicit optional_storage(in_place_t /*unused*/, Args&&... args)
            : m_has_value(true)
        {
          m_val.set(rsl::forward<Args>(args)...);
        }

        template <typename U, typename... Args, typename = enable_if_t<is_constructible_v<T, rsl::initializer_list<U>&, Args&&...>>>
        constexpr explicit optional_storage(in_place_t /*unused*/, rsl::initializer_list<U> ilist, Args&&... args)
            : m_has_value(true)
        {
          m_val.set(ilist, rsl::forward<Args>(args)...);
        }

        // default to make optional trivially destructible
        ~optional_storage() = default;

        template <typename... Args>
        constexpr void construct_value(Args&&... args)
        {
          m_val.set(rsl::forward<Args>(args)...);
        }
        constexpr void destroy_value() {}

        constexpr bool has_value() const
        {
          return m_has_value;
        }
        constexpr void has_value(bool newHasValue)
        {
          m_has_value = newHasValue;
        }
        constexpr void set_val(const value_type& val)
        {
          m_val.set(&val);
        }
        constexpr void set_val(value_type&& val)
        {
          m_val.set(&val);
        }
        constexpr const value_type& val() const
        {
          return *m_val.get();
        }
        constexpr value_type& val()
        {
          return *m_val.get();
        }

        constexpr void swap(optional_storage<value_type>& other)
        {
          if(has_value() == other.has_value())
          {
            if(has_value())
            {
              rsl::swap(val(), other.val());
            }
          }
          else
          {
            if(has_value())
            {
              other.construct_value(rsl::move(val()));
              destroy_value();
            }
            else
            {
              construct_value(rsl::move(other.val()));
              other.destroy_value();
            }

            rsl::swap(m_has_value, other.m_has_value);
          }
        }

      private:
        bool m_has_value;
        aligned_storage<value_type> m_val {};
      };
    } // namespace internal

    template <typename T>
    class optional
    {
    private:
      using storage_type = internal::optional_storage<T>;

      template <typename T2>
      using allow_direct_conversion =
          bool_constant<conjunction_v<negation<is_same<remove_cvref_t<T2>, optional>>, negation<is_same<remove_cvref_t<T2>, in_place_t>>, is_constructible<T, T2>, negation<is_same<bool, T>>>>; // solving issues when assigning opt<int> to opt<bool>
    public:
      using value_type = T;

      static_assert(!rsl::is_reference_v<T>, "an optional holding a reference is ill-formed");
      static_assert(!rsl::is_same_v<T, in_place_t>, "an optional holding an \"in place\" type is ill-formed");
      static_assert(!rsl::is_same_v<T, nullopt_t>, "an optional holding a nullopt_t is ill-formed");

      // constructs an object that does not contain a value
      constexpr optional() = default;
      // constructs an object that does not contain a value
      constexpr optional(nullopt_t /*unused*/) // NOLINT(google-explicit-constructor)
          : m_storage()
      {
      }
      // copy constructor, if other contains a value, it's copied in to this
      constexpr optional(const optional& other)
      {
        m_storage.has_value(other.has_value());

        if(m_storage.has_value())
        {
          const auto& other_value = other.value();
          m_storage.construct_value(other_value);
        }
      }
      // move constructor, if other contains a value, it's moved in to this.
      constexpr optional(optional&& other)
      {
        m_storage.has_value(other.has_value());

        if(m_storage.has_value())
        {
          auto& other_value = other.value();
          m_storage.construct_value(rsl::move(other_value));
        }
      }
      // converting copy constructor, if other contains a value, it's copied in to this
      template <typename U>
      explicit constexpr optional(const optional<U>& other)
      {
        m_storage.has_value(other.has_value());

        if(m_storage.has_value())
        {
          const auto& other_value = other.value();
          m_storage.construct_value(other_value);
        }
      }
      // converting move constructor, if other contains a value, it's moved in to this
      template <typename U>
      constexpr optional(optional<U>&& other) // NOLINT(google-explicit-constructor)
      {
        m_storage.has_value(other.has_value());

        if(m_storage.has_value())
        {
          auto& other_value = other.value();
          m_storage.construct_value(rsl::move(other_value));
        }
      }
      // construct an optional object that contains a value initialized with the arguments "arg"
      template <typename... Args>
      constexpr explicit optional(in_place_t /*unused*/, Args&&... args)
          : m_storage(in_place, rsl::forward<Args>(args)...)
      {
      }

      // construct an optional object that contains a value initialized with the arguments ilist and "arg"
      template <typename U, typename... Args, typename = enable_if_t<is_constructible_v<T, rsl::initializer_list<U>&, Args&&...>>>
      constexpr optional(in_place_t /*unused*/, rsl::initializer_list<U> ilist, Args&&... args)
          : m_storage(in_place, ilist, rsl::forward<Args>(args)...)
      {
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (12/Oct/2022)
      // copies val into this optional
      // this overload is provided to avoid conflicts with optional bools
      optional(const T& val) // NOLINT(google-explicit-constructor)
          : m_storage(val)
      {
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (12/Oct/2022)
      // moves val into this optional
      // this overload is provided to avoid conflicts with optional bools
      optional(T&& val) // NOLINT(google-explicit-constructor)
          : m_storage(rsl::move(val))
      {
      }
      // construct an object initialized from the argument provided
      template <typename U = T, enable_if_t<allow_direct_conversion<U>::value, bool> = true>
      optional(U&& value) // NOLINT(google-explicit-constructor)
          : m_storage(rsl::forward<U>(value))
      {
      }

      // if this contains a value and is not trivially destructible, it calls its constructor
      ~optional() = default;

      // if this contains a value, it's destroyed, afterwards this object contains no value
      constexpr optional& operator=(nullopt_t /*unused*/)
      {
        reset();
        return *this;
      }
      // if other contains a value, it's copied in to this. if this contains a value, it's copy assignment operator is called
      constexpr optional& operator=(const optional& other)
      {
        auto* other_value = reinterpret_cast<const T*>(rsl::addressof(other.val())); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        if(m_storage.has_value() == other.has_value())
        {
          if(m_storage.has_value())
          {
            m_storage.val() = *other_value;
          }
        }
        else
        {
          if(m_storage.has_value())
          {
            m_storage.destroy_value();
            m_storage.has_value(false);
          }
          else
          {
            m_storage.construct_value(*other_value);
            m_storage.has_value(true);
          }
        }
        return *this;
      }
      // if other contains a value, it's moved in to this. if this contains a value, it's move assignment operator is called
      constexpr optional& operator=(optional&& other)
      {
        auto* other_value = reinterpret_cast<const T*>(rsl::addressof(other.value())); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        if(m_storage.has_value() == other.has_value())
        {
          if(m_storage.has_value())
          {
            m_storage.set_val(rsl::move(*other_value));
          }
        }
        else
        {
          if(m_storage.has_value())
          {
            m_storage.destroy_value();
            m_storage.has_value(false);
          }
          else
          {
            m_storage.construct_value(rsl::move(*other_value));
            m_storage.has_value(true);
          }
        }
        return *this;
      }
      // if this contains a value, it's copy or move assigned. otherwise a new value is constructed.
      template <typename U, typename = rsl::enable_if_t<rsl::is_same_v<rsl::decay_t<U>, T>>>
      constexpr optional& operator=(U&& u)
      {
        if(m_storage.has_value())
        {
          m_storage.val() = rsl::forward<U>(u);
        }
        else
        {
          m_storage.has_value(true);
          m_storage.construct_value(rsl::forward<U>(u));
        }
        return *this;
      }
      // if other contains a value, it's copied in to this. if this contains a value, it's copy assignment operator is called
      template <typename U>
      constexpr optional& operator=(const optional<U>& other)
      {
        auto* other_value = reinterpret_cast<const T*>(rsl::addressof(other.val())); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        if(m_storage.has_value() == other.has_value())
        {
          if(m_storage.has_value())
          {
            m_storage.val() = *other_value;
          }
        }
        else
        {
          if(m_storage.has_value())
          {
            m_storage.destroy_value();
            m_storage.has_value(false);
          }
          else
          {
            m_storage.construct_value(*other_value);
            m_storage.has_value(true);
          }
        }
        return *this;
      }
      // if other contains a value, it's moved in to this. if this contains a value, it's move assignment operator is called
      template <typename U>
      constexpr optional& operator=(optional<U>&& other)
      {
        auto* other_value = reinterpret_cast<const T*>(rsl::addressof(other.val())); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        if(m_storage.has_value() == other.has_value())
        {
          if(m_storage.has_value())
          {
            m_storage.val() = rsl::move(*other_value);
          }
        }
        else
        {
          if(m_storage.has_value())
          {
            m_storage.destroy_value();
            m_storage.has_value(false);
          }
          else
          {
            m_storage.construct_value(rsl::move(*other_value));
            m_storage.has_value(true);
          }
        }
        return *this;
      }

      // returns a pointer to the contained value
      constexpr const T* operator->() const
      {
        return rsl::addressof(m_storage.val());
      }
      // returns a pointer to the contained value
      constexpr T* operator->()
      {
        return rsl::addressof(m_storage.val());
      }
      // returns a reference to the contained value
      constexpr const T& operator*() const&
      {
        return m_storage.val();
      }
      // returns a reference to the contained value
      constexpr T& operator*() &
      {
        return m_storage.val();
      }
      constexpr const T&& operator*() const&&
      {
        return m_storage.val();
      }
      // returns a reference to the contained value
      constexpr T&& operator*() &&
      {
        return m_storage.val();
      }

      // checks whether this contains a value
      constexpr explicit operator bool() const
      {
        return m_storage.has_value();
      }
      // checks whether this contains a value
      constexpr bool has_value() const
      {
        return m_storage.has_value();
      }

      // if this contains a value, returns a reference to the contained value.
      // asserts otherwise
      constexpr T& value() &
      {
        REX_ASSERT_X(has_value(), "bad optional access!");
        return m_storage.val();
      }
      // if this contains a value, returns a reference to the contained value.
      // asserts otherwise
      constexpr const T& value() const&
      {
        REX_ASSERT_X(has_value(), "bad optional access!");
        return m_storage.val();
      }
      // if this contains a value, returns a reference to the contained value.
      // asserts otherwise
      constexpr T&& value() &&
      {
        REX_ASSERT_X(has_value(), "bad optional access!");
        return rsl::move(m_storage.val());
      }
      // if this contains a value, returns a reference to the contained value.
      // asserts otherwise
      constexpr const T&& value() const&&
      {
        REX_ASSERT_X(has_value(), "bad optional access!");
        return m_storage.val();
      }

      // returns the contained value if this has one, returns default value otherwise
      template <typename U>
      constexpr value_type value_or(U&& defaultValue) const&
      {
        return m_storage.has_value() ? m_storage.val() : static_cast<value_type>(rsl::forward<U>(defaultValue));
      }
      // returns the contained value if this has one, returns default value otherwise
      template <typename U>
      constexpr value_type value_or(U&& defaultValue) &&
      {
        return m_storage.has_value() ? rsl::move(m_storage.val()) : static_cast<value_type>(defaultValue);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (18/Jul/2022)
      // because rex does not yet support C++20, we don't define the following functions
      // template< class F >
      // constexpr auto and_then( F&& f ) &;
      // template< class F >
      // constexpr auto and_then( F&& f ) const&;
      // template< class F >
      // constexpr auto and_then( F&& f ) &&;
      // template< class F >
      // constexpr auto and_then( F&& f ) const&&;

      // template< class F >
      // constexpr auto transform( F&& f ) &;
      // template< class F >
      // constexpr auto transform( F&& f ) const&;
      // template< class F >
      // constexpr auto transform( F&& f ) &&;
      // template< class F >
      // constexpr auto transform( F&& f ) const&&;

      // template< class F >
      // constexpr optional or_else( F&& f ) const&;
      // template< class F >
      // constexpr optional or_else( F&& f ) &&;

      // constructs the contained value in place.
      // if this already contains a value before the call, the contained value is destroyed by calling its destructor
      template <typename... Args>
      constexpr T& emplace(Args&&... args)
      {
        if(has_value())
        {
          m_storage.destroy_value();
        }

        m_storage.construct_value(rsl::forward<Args>(args)...);
        m_storage.has_value(true);
        return m_storage.val();
      }
      // constructs the contained value in place.
      // if this already contains a value before the call, the contained value is destroyed by calling its destructor
      template <typename U, typename... Args, enable_if_t<is_constructible_v<T, rsl::initializer_list<U>&, Args...>, bool> = true>
      constexpr T& emplace(rsl::initializer_list<U> ilist, Args&&... args)
      {
        return emplace(ilist, rsl::forward<Args>(args)...);
      }

      // swaps the contents with those of other
      constexpr void swap(optional& other)
      {
        m_storage.swap(other.m_storage);
      }

      // if this contains a value, destroy that value by calling its constructor.
      constexpr void reset()
      {
        if(has_value())
        {
          m_storage.destroy_value();
          m_storage.has_value(false);
        }
      }

    private:
      storage_type m_storage {};
    };

    // compares two optional objects
    template <typename T, typename U>
    constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs)
    {
      if(lhs.has_value() != rhs.has_value())
      {
        return false;
      }

      if(!lhs.has_value())
      {
        return true;
      }

      return *lhs == *rhs;
    }
    // compares two optional objects
    template <typename T, typename U>
    constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
    {
      return !(lhs == rhs);
    }
    // compares two optional objects
    template <typename T, typename U>
    constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs)
    {
      if(!rhs.has_value())
      {
        return false;
      }

      if(!lhs.has_value())
      {
        return true;
      }

      return *lhs < *rhs;
    }
    // compares two optional objects
    template <typename T, typename U>
    constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
    {
      return !(*rhs < *lhs);
    }
    // compares two optional objects
    template <typename T, typename U>
    constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs)
    {
      return rhs < lhs;
    }
    // compares two optional objects
    template <typename T, typename U>
    constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
    {
      return !(lhs < rhs);
    }

    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator==(const optional<T>& opt, nullopt_t /*unused*/)
    {
      return !opt;
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator==(nullopt_t /*unused*/, const optional<T>& opt)
    {
      return !opt;
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator!=(const optional<T>& opt, nullopt_t /*unused*/)
    {
      return bool(opt);
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator!=(nullopt_t /*unused*/, const optional<T>& opt)
    {
      return bool(opt);
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator<(const optional<T>& /*unused*/, nullopt_t /*unused*/)
    {
      return false;
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator<(nullopt_t /*unused*/, const optional<T>& opt)
    {
      return bool(opt);
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator<=(const optional<T>& opt, nullopt_t /*unused*/)
    {
      return !opt;
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator<=(nullopt_t /*unused*/, const optional<T>& /*unused*/)
    {
      return true;
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator>(const optional<T>& opt, nullopt_t /*unused*/)
    {
      return bool(opt);
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator>(nullopt_t /*unused*/, const optional<T>& /*unused*/)
    {
      return false;
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator>=(const optional<T>& /*unused*/, nullopt_t /*unused*/)
    {
      return true;
    }
    // compares optional object with nullopt
    template <typename T>
    constexpr bool operator>=(nullopt_t /*unused*/, const optional<T>& opt)
    {
      return !opt;
    }

    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator==(const optional<T>& opt, const U& value)
    {
      if(opt.has_value())
      {
        return *opt == value;
      }

      return false;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator==(const U& value, const optional<T>& opt)
    {
      if(opt.has_value())
      {
        return value == *opt;
      }

      return false;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator!=(const optional<T>& opt, const U& value)
    {
      if(opt.has_value())
      {
        return !(*opt == value);
      }

      return true;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator!=(const U& value, const optional<T>& opt)
    {
      if(opt.has_value())
      {
        return !(value == opt);
      }

      return true;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator<(const optional<T>& opt, const U& value)
    {
      if(opt.has_value())
      {
        return *opt < value;
      }

      return true;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator<(const U& value, const optional<T>& opt)
    {
      if(opt.has_value())
      {
        return value < *opt;
      }

      return false;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator<=(const optional<T>& opt, const U& value)
    {
      return !(opt > value);
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator<=(const U& value, const optional<T>& opt)
    {
      return !(value > opt);
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator>(const optional<T>& opt, const U& value)
    {
      if(opt.has_value())
      {
        return value < *opt;
      }

      return false;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator>(const U& value, const optional<T>& opt)
    {
      if(opt.has_value())
      {
        return *opt < value;
      }

      return true;
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator>=(const optional<T>& opt, const U& value)
    {
      return !(opt < value);
    }
    // compares optional object with value
    template <typename T, typename U>
    constexpr bool operator>=(const U& value, const optional<T>& opt)
    {
      return !(value < opt);
    }

    // creates an optional object from value
    template <typename T>
    constexpr optional<decay_t<T>> make_optional(T&& value)
    {
      return optional<decay_t<T>>(rsl::forward<T>(value));
    }
    // creates an optional object in place using args
    template <typename T, typename... Args>
    constexpr optional<T> make_optional(Args&&... args)
    {
      return optional<T>(in_place, rsl::forward<T>(rsl::forward<Args>(args)...));
    }
    // creates an optional object in place using ilist and args
    template <typename T, typename U, typename... Args>
    constexpr optional<T> make_optional(rsl::initializer_list<U> ilist, Args&&... args)
    {
      return optional<T>(in_place, ilist, rsl::forward<Args>(args)...);
    }

    // swaps 2 optional objects with each other
    template <typename T>
    constexpr void swap(optional<T>& lhs, optional<T>& rhs)
    {
      return lhs.swap(rhs);
    }

    // deduction guides
    template <typename T>
    optional(T) -> optional<T>;

    template <typename T>
    struct hash;

    template <typename T>
    struct hash<optional<T>>
    {
      hash_result operator()(const optional<T>& opt) const
      {
        if(opt.has_value())
        {
          return hash<T> {}(*opt);
        }

        return static_cast<hash_result>(0);
      }
    };

  } // namespace v1
} // namespace rsl
