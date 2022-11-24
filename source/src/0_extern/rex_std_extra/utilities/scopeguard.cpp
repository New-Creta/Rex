// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: scopeguard.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std_extra/utilities/scopeguard.h"

rsl::ScopeGuard::ScopeGuard(rsl::function<void()> func)
    : m_function(func)
{}

rsl::ScopeGuard::~ScopeGuard()
{
    if (!m_is_released)
    {
        m_function();
    }
}

void rsl::ScopeGuard::release()
{
    m_is_released = true;
}