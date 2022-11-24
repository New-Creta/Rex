// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: key_value_pair.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
    template <typename Key, typename Value>
    struct key_value_pair
    {
        using key_type = Key;
        using value_type = Value;

        Key key;
        Value value;
    };
}