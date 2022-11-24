// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: unique_ptr.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/internal/functional/hash.h"
#include "rex_std/internal/functional/less.h"
#include "rex_std/internal/memory/default_delete.h"
#include "rex_std/internal/memory/nullptr.h"
#include "rex_std/internal/type_traits/common_type.h"
#include "rex_std/internal/type_traits/conjunction.h"
#include "rex_std/internal/type_traits/is_default_constructible.h"
#include "rex_std/internal/type_traits/is_move_constructible.h"
#include "rex_std/internal/type_traits/is_pointer.h"
#include "rex_std/internal/type_traits/is_reference.h"
#include "rex_std/internal/type_traits/negation.h"
#include "rex_std/internal/type_traits/remove_reference.h"
#include "rex_std/internal/utility/exchange.h"
#include "rex_std/ostream.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, typename Deleter>
      class unique_pointer_type
      {
        template <typename U>
        static typename U::pointer test(typename U::pointer*);

        template <typename U>
        static T* test(...);

      public:
        using type = decltype(test<typename rsl::remove_reference<Deleter>::type>(0));
      };
      template <typename Deleter>
      using UniquePtrEnableDefault = enable_if_t<conjunction_v<negation<is_pointer<Deleter>>, is_default_constructible<Deleter>>, int>;

    } // namespace internal

    template <typename T, typename Deleter = default_delete<T>>
    class unique_ptr
    {
    public:
      using pointer         = typename internal::unique_pointer_type<T, remove_reference_t<Deleter>>::type;
      using const_pointer   = typename internal::unique_pointer_type<const T, remove_reference_t<Deleter>>::type; /// RSL Comment: Not in ISO C++ Standard at time of writing (24/Aug/2022)
      using reference       = T&;                                                                                 /// RSL Comment: Not in ISO C++ Standard at time of writing (24/Aug/2022)
      using const_reference = const T&;                                                                           /// RSL Comment: Not in ISO C++ Standard at time of writing (24/Aug/2022)
      using element_type    = T;
      using deleter_type    = Deleter;

      // constructs a unique_ptr that owns nothing
      template <typename Deleter2 = Deleter, internal::UniquePtrEnableDefault<Deleter2> = 0>
      constexpr unique_ptr()
          : m_cp_ptr_and_deleter(nullptr)
      {
      }
      // constructs a unique_ptr that owns nothing
      template <typename Deleter2 = Deleter, internal::UniquePtrEnableDefault<Deleter2> = 0>
      constexpr unique_ptr(rsl::nullptr_t) // NOLINT(google-explicit-constructor): this needs to be explicit
          : m_cp_ptr_and_deleter(nullptr)
      {
      }
      // construct a unique_ptr that owns ptr
      template <typename Deleter2 = Deleter, internal::UniquePtrEnableDefault<Deleter2> = 0>
      constexpr explicit unique_ptr(pointer ptr)
          : m_cp_ptr_and_deleter(ptr)
      {
      }
      // construct a unique_ptr that owns ptr and copy constructs the deleter
      template <typename Deleter2 = Deleter, enable_if_t<is_constructible_v<Deleter2, const Deleter2&>, bool> = true>
      unique_ptr(pointer ptr, const Deleter& deleter)
          : m_cp_ptr_and_deleter(ptr, deleter)
      {
      }
      // construct a unique_ptr that owns ptr and copy constructs the deleter
      template <typename Deleter2, enable_if_t<conjunction_v<negation<is_reference<Deleter2>>, is_constructible<Deleter2, Deleter2>>, bool> = true>
      unique_ptr(pointer ptr, Deleter&& deleter)
          : m_cp_ptr_and_deleter(ptr, rsl::move(deleter))
      {
      }
      template <typename Deleter2, enable_if_t<conjunction_v<is_reference<Deleter2>, is_constructible<Deleter2, remove_reference_t<Deleter2>>>, bool> = true>
      unique_ptr(pointer, remove_reference_t<Deleter>&&) = delete;
      // dleters the copy constructor
      unique_ptr(const unique_ptr&) = delete;
      // construct a unique_ptr that takes ownership of the  ptr in the other unique_ptr.
      // perfectly forwards the deleter of the other unique_ptr.
      unique_ptr(unique_ptr&& right)
          : m_cp_ptr_and_deleter(right.release(), rsl::forward<Deleter>(right.get_deleter()))
      {
      }
      // construct a unique_ptr that owns ptr and copy constructs the deleter
      template <
          typename T2, typename Deleter2,
          enable_if_t<conjunction_v<negation<is_array<T2>>, is_convertible<typename unique_ptr<T2, Deleter2>::pointer, pointer>, conditional_t<is_reference_v<Deleter>, is_same<Deleter, Deleter>, is_convertible<Deleter2, Deleter>>>, bool> = true>
      explicit unique_ptr(unique_ptr<T2, Deleter2>&& right)
          : m_cp_ptr_and_deleter(right.release(), rsl::forward<Deleter2>(right.get_deleter()))
      {
      }

      // the owner object is destroyed via the deleter
      ~unique_ptr()
      {
        delete_ptr();
      }

      // transfers ownership from other to this
      unique_ptr& operator=(unique_ptr&& other) noexcept
      {
        reset(other.release());
        m_cp_ptr_and_deleter.second() = rsl::forward<Deleter>(other.get_deleter());
        return *this;
      }
      // converting assignment operator
      template <typename T2, rsl::enable_if_t<rsl::is_convertible_v<T, T2>> = 0>
      unique_ptr& operator=(unique_ptr<T2>&& other)
      {
        reset(other.release());
        return *this;
      }
      // effectively is the same as calling reset
      unique_ptr& operator=(rsl::nullptr_t)
      {
        reset();
        return *this;
      }
      unique_ptr& operator=(const unique_ptr&) = delete;

      // releases the ownership of the managed object
      REX_NO_DISCARD pointer release()
      {
        return rsl::exchange(m_cp_ptr_and_deleter.first(), pointer());
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Aug/2022)
      // in the standard the following steps are taken:
      // 1) copy holding pointer to a temp
      // 2) assign new pointer
      // 3) destroy old pointer if it wasn't nullptr.
      // the reason for this is because the call to get_deleter() may destroy *this.
      // I can't think of a way when this would happen, so in Rex Standard, if such a call would destroy *this
      // that results in undefined behavior, hence why we do things a bit differently.
      // destroys the currently held pointer, assigns a new one
      void reset(T* ptr = pointer())
      {
        m_cp_ptr_and_deleter.second()(m_cp_ptr_and_deleter.first());
        m_cp_ptr_and_deleter.first() = ptr;
      }

      // swaps the managed objects and their deleter
      void swap(unique_ptr& other)
      {
        swap(m_cp_ptr_and_deleter.first(), other.m_cp_ptr_and_deleter.first());
        swap(m_cp_ptr_and_deleter.second(), other.m_cp_ptr_and_deleter.second());
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Aug/2022)
      // in the standard, unique ptr does not propagate const, in rex it does
      // in the standard, the way to get a const T pointer is to create a unique_ptr<const T>
      // however, pointers where the object they're pointing to is const rather than the pointer itself
      // are much more common, so that's why rex standard propagates const.
      // returns a pointer to the managed object
      REX_NO_DISCARD const_pointer get() const
      {
        return m_cp_ptr_and_deleter.first();
      }
      // returns a pointer to the managed object
      REX_NO_DISCARD pointer get()
      {
        return m_cp_ptr_and_deleter.first();
      }

      // returns the deleter object used to destroy the managed object
      REX_NO_DISCARD const deleter_type& get_deleter() const
      {
        return m_cp_ptr_and_deleter.second();
      }
      // returns the deleter object used to destroy the managed object
      REX_NO_DISCARD deleter_type& get_deleter()
      {
        return m_cp_ptr_and_deleter.second();
      }

      // checks whether this owns an object.
      explicit operator bool() const
      {
        return m_cp_ptr_and_deleter.first() != nullptr;
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Aug/2022)
      // the standard doesn't propagate const, rex does
      // returns a reference to the managed object
      const_reference operator*() const
      {
        return *m_cp_ptr_and_deleter.first();
      }
      // returns a reference to the managed object
      reference operator*()
      {
        return *m_cp_ptr_and_deleter.first();
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (03/Aug/2022)
      // the standard doesn't propagate const, rex does
      // in the standard, the way to get a const T pointer is to create a unique_ptr<const T>
      // however, pointers where the object they're pointing to is const rather than the pointer itself
      // are much more common, so that's why rex standard propagates const.

      // returns a pointer to the managed object
      pointer operator->()
      {
        return m_cp_ptr_and_deleter.first();
      }
      // returns a pointer to the managed object
      const_pointer operator->() const
      {
        return m_cp_ptr_and_deleter.first();
      }

    private:
      void delete_ptr()
      {
        get_deleter()(get());
      }

    private:
      compressed_pair<pointer, deleter_type> m_cp_ptr_and_deleter {};
    };

    template <typename T1, typename D1, typename T2, typename D2>
    bool operator==(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs)
    {
      return lhs.get() == rhs.get();
    }
    template <typename T1, typename D1, typename T2, typename D2>
    bool operator!=(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs)
    {
      return lhs.get() != rhs.get();
    }
    template <typename T1, typename D1, typename T2, typename D2>
    bool operator<(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs)
    {
      using ptr1   = typename unique_ptr<T1, D1>::pointer;
      using ptr2   = typename unique_ptr<T2, D2>::pointer;
      using common = common_type_t<ptr1, ptr2>;
      return less<common> {}(lhs.get(), rhs.get());
    }
    template <typename T1, typename D1, typename T2, typename D2>
    bool operator>=(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs)
    {
      return !(lhs < rhs);
    }
    template <typename T1, typename D1, typename T2, typename D2>
    bool operator>(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs)
    {
      return rhs < lhs;
    }
    template <typename T1, typename D1, typename T2, typename D2>
    bool operator<=(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs)
    {
      return !(rhs < lhs);
    }

    template <typename T, typename Deleter>
    bool operator==(const unique_ptr<T, Deleter>& lhs, nullptr_t)
    {
      return !lhs;
    }
    template <typename T, typename Deleter>
    bool operator==(nullptr_t, const unique_ptr<T, Deleter>& rhs)
    {
      return !rhs;
    }
    template <typename T, typename Deleter>
    bool operator!=(const unique_ptr<T, Deleter>& lhs, nullptr_t rhs)
    {
      return !(lhs == rhs);
    }
    template <typename T, typename Deleter>
    bool operator!=(nullptr_t lhs, const unique_ptr<T, Deleter>& rhs)
    {
      return !(lhs == rhs);
    }

    template <typename T, typename Deleter>
    bool operator<(const unique_ptr<T, Deleter>& lhs, nullptr_t rhs)
    {
      using ptr = typename unique_ptr<T, Deleter>::pointer;
      return less<ptr> {}(lhs.get(), rhs);
    }
    template <typename T, typename Deleter>
    bool operator<(nullptr_t lhs, const unique_ptr<T, Deleter>& rhs)
    {
      using ptr = typename unique_ptr<T, Deleter>::pointer;
      return less<ptr> {}(lhs, rhs.get());
    }
    template <typename T, typename Deleter>
    bool operator>=(const unique_ptr<T, Deleter>& lhs, nullptr_t rhs)
    {
      return !(lhs < rhs);
    }
    template <typename T, typename Deleter>
    bool operator>=(nullptr_t lhs, const unique_ptr<T, Deleter>& rhs)
    {
      return !(lhs < rhs);
    }
    template <typename T, typename Deleter>
    bool operator>(const unique_ptr<T, Deleter>& lhs, nullptr_t rhs)
    {
      return rhs < lhs;
    }
    template <typename T, typename Deleter>
    bool operator>(nullptr_t lhs, const unique_ptr<T, Deleter>& rhs)
    {
      return rhs < lhs;
    }
    template <typename T, typename Deleter>
    bool operator<=(const unique_ptr<T, Deleter>& lhs, nullptr_t rhs)
    {
      return !(rhs < lhs);
    }
    template <typename T, typename Deleter>
    bool operator<=(nullptr_t lhs, const unique_ptr<T, Deleter>& rhs)
    {
      return !(rhs < lhs);
    }

    template <typename T, typename... Args, rsl::enable_if_t<!rsl::is_array_v<T>, bool> = true>
    REX_NO_DISCARD unique_ptr<T> make_unique(Args&&... args)
    {
      return unique_ptr<T>(new T(rsl::forward<Args>(args)...));
    }
    template <typename T>
    unique_ptr<T> make_unique_for_overwrite()
    {
      return unique_ptr<T>(new T);
    }

    // template <typename CharType, typename Traits>
    // class basic_ostream;

    template <typename CharType, typename Traits, typename Y, typename D>
    basic_ostream<CharType, Traits>& operator<<(basic_ostream<CharType, Traits>& os, const unique_ptr<Y, D>& p)
    {
      os << p.get();
      return os;
    }

    template <typename T, typename Deleter>
    void swap(unique_ptr<T, Deleter>& lhs, unique_ptr<T, Deleter>& rhs)
    {
      lhs.swap(rhs);
    }

    template <typename T, typename D>
    struct hash<unique_ptr<T, D>>
    {
      hash_result operator()(const unique_ptr<T, D>& ptr) const
      {
        hash<typename decltype(ptr)::pointer> new_hash;
        return new_hash(ptr.get());
      }
    };

  } // namespace v1
} // namespace rsl
