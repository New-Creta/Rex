

#pragma once

#include <atomic>
#include <utility>
#include "rex_std/algorithm.h"
#include "rex_std/atomic.h"
// null, no cost dummy "mutex" and dummy "atomic" int

namespace rexlog {
namespace details {
struct null_mutex
{
    void lock() const {}
    void unlock() const {}
};

struct null_atomic_int
{
    int value;
    null_atomic_int() = default;

    explicit null_atomic_int(int new_value)
        : value(new_value)
    {}

    int load(rsl::memory_order = rsl::memory_order::relaxed) const
    {
        return value;
    }

    void store(int new_value, rsl::memory_order = rsl::memory_order::relaxed)
    {
        value = new_value;
    }

    int exchange(int new_value, rsl::memory_order = rsl::memory_order::relaxed)
    {
        rsl::swap(new_value, value);
        return new_value; // return value before the call
    }
};

} // namespace details
} // namespace rexlog
