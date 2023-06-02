

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
  } // namespace details
} // namespace rexlog
