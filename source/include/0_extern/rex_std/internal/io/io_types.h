// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: io_types.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
    using streamoff = card32; /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
    using streamsize = card32; /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
    
    using streampos = card32; /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
    using wstreampos = card32; /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
    using u16streampos = card32; /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
    using u32streampos = card32; /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)

    using wint_t = int32; /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
}