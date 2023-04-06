

#pragma once

#include "rex_std/algorithm.h"
#include "rex_std/atomic.h"

#include <atomic>
#include <utility>
// null, no cost dummy "mutex" and dummy "atomic" int

namespace rexlog
{
  namespace details
  {
    struct NullMutex
    {
      void lock() const {}
      void unlock() const {}
    };

    struct NullAtomicInt
    {
      int value;
      NullAtomicInt() = default;

      explicit NullAtomicInt(int newValue)
          : value(newValue)
      {
      }

      int load(rsl::memory_order  /*unused*/= rsl::memory_order::relaxed) const
      {
        return value;
      }

      void store(int newValue, rsl::memory_order  /*unused*/= rsl::memory_order::relaxed)
      {
        value = newValue;
      }

      int exchange(int newValue, rsl::memory_order  /*unused*/= rsl::memory_order::relaxed)
      {
        rsl::swap(newValue, value);
        return newValue; // return value before the call
      }
    };

  } // namespace details
} // namespace rexlog
