// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: hash.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/types.h"

#include "rex_std/internal/type_traits/is_arithmetic.h"

/// [22/Aug/2022] RSL Comment: there's a hash functor in functional

namespace rsl
{
    using HashID = uint64;

    // Uses MSVC hash implemenation
    // Do we need to hash arithmetic types?
    template <typename T>
    struct hash
    {
        static_assert(is_arithmetic_v<T>, "base hash implementation only supports arithmetic types!");

        REX_NO_DISCARD HashID operator()(const T& key) const
        {
            fnv1a_append_value(s_fnv_offset_basis, key);
        }

    private:
        // accumulate range [first, first + count) into partial FNV-1a hash val
        REX_NO_DISCARD HashID fnv1a_append_bytes(HashID val, const unsigned char* const first, const HashID count) noexcept 
        {
            for (HashID idx = 0; idx < count; ++idx) 
            {
                val ^= static_cast<HashID>(first[idx]);
                val *= s_fnv_prime;
            }

            return val;
        }

        REX_NO_DISCARD HashID fnv1a_append_value(const HashID val, const T& key)
        {
            return fnv1a_append_bytes(val, &reinterpret_cast<const unsigned char&>(key), sizeof(T));
        }

    private:
        constexpr static HashID s_fnv_offset_basis = 14695981039346656037ULL;
        constexpr static HashID s_fnv_prime = 14695981039346656037ULL;
    };
}

