// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_id.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/functional/hash.h"

namespace rsl
{
    using AssertIDHash = hash_result;

    class AssertID
    {
    public:
        constexpr AssertID(const char8* severity, const char8* file, const char8* function_sig, const char8* expr, const char8* msg)
            : m_hash(0)
        {
            m_hash = internal::hash_combine(m_hash, internal::hash(severity));
            m_hash = internal::hash_combine(m_hash, internal::hash(file));
            m_hash = internal::hash_combine(m_hash, internal::hash(function_sig));
            m_hash = internal::hash_combine(m_hash, internal::hash(expr));
            m_hash = internal::hash_combine(m_hash, internal::hash(msg));
        }

        constexpr AssertIDHash hash() const
        {
            return m_hash;
        }

    private:
        AssertIDHash m_hash;
    };
}