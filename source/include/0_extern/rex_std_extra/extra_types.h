// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: extra_types.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "utilities/strong_type.h"

namespace rsl
{
    inline namespace extra
    {
        REX_CREATE_STRONG_TYPE(Capacity, card32);
        REX_CREATE_STRONG_TYPE(Size, card32);
    }
}

CREATE_LITERAL_FOR_STRONG_TYPE(rsl::Capacity, _cap);
CREATE_LITERAL_FOR_STRONG_TYPE(rsl::Size, _size);