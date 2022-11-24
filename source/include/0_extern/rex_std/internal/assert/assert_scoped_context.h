// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_scoped_context.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "assert_context.h"

namespace rsl
{
    class AssertContext;

    namespace assert
    {
        class ScopedAssertContext
        {
        public:
            ScopedAssertContext(const AssertContext& context);
            ~ScopedAssertContext();

        private:
            AssertContext m_context;
        };
    }
}