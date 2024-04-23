

#pragma once

#include "rex_std/algorithm.h"
#include "rex_std/atomic.h"

// null, no cost dummy "mutex" and dummy "atomic" int

namespace rex
{
  namespace log
  {
    namespace details
    {
      struct NullMutex
      {
        void lock() const {}
        void unlock() const {}
      };
    } // namespace details
  }   // namespace log
} // namespace rex
