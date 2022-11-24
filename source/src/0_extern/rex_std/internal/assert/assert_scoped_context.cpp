// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_scoped_context.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/assert/assert_scoped_context.h"

rsl::assert::ScopedAssertContext::ScopedAssertContext(const AssertContext& context)
    : m_context(context)
{}

rsl::assert::ScopedAssertContext::~ScopedAssertContext()
{}