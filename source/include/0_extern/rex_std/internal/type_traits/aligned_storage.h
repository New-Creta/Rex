// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: aligned_storage.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/memory/byte.h"
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/internal/utility/forward.h"

#include <new> // needed for placement new

namespace rsl
{
  inline namespace v1
  {

    /// RSL Comment: Different from ISO C++ Standard at time of writing (26/Aug/2022)
    template <typename T>
    class aligned_storage
    {
    public:
      constexpr aligned_storage()
          : m_buff()
      {
      }
      constexpr aligned_storage(const aligned_storage& other) // NOLINT(cppcoreguidelines-pro-type-member-init)
      {
        memcpy(m_buff, other.m_buff, sizeof(T));
      }
      constexpr aligned_storage(aligned_storage&& other) // NOLINT(cppcoreguidelines-pro-type-member-init)
      {
        memcpy(m_buff, other.m_buff, sizeof(T));
      }

      ~aligned_storage() = default;

      constexpr aligned_storage& operator=(const aligned_storage& other) // NOLINT(bugprone-unhandled-self-assignment)
      {
        REX_ASSERT_X(this != addressof(other), "Can't copy to yourself");
        memcpy(m_buff, other.m_buff, sizeof(T));
        return *this;
      }
      constexpr aligned_storage& operator=(aligned_storage&& other)
      {
        REX_ASSERT_X(this != addressof(other), "Can't move to yourself");
        memcpy(m_buff, other.m_buff, sizeof(T));
        return *this;
      }

      template <typename... Args>
      constexpr void set(Args&&... args)
      {
        new(static_cast<void*>(m_buff)) T(rsl::forward<Args>(args)...);
      }
      constexpr void set(const T* newVal)
      {
        rsl::memcpy(m_buff, newVal, sizeof(T));
      }
      constexpr const T* get() const
      {
        return reinterpret_cast<const T*>(m_buff); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      }
      constexpr T* get()
      {
        return reinterpret_cast<T*>(m_buff); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      }

    private:
      alignas(T) rsl::byte m_buff[sizeof(T)] = {}; // NOLINT(modernize-avoid-c-arrays)
    };

  } // namespace v1
} // namespace rsl
