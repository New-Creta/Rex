// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: iterator_tags.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    struct input_iterator_tag
    {
    };

    struct output_iterator_tag
    {
    };

    struct forward_iterator_tag : input_iterator_tag
    {
    };

    struct bidirectional_iterator_tag : forward_iterator_tag
    {
    };

    struct random_access_iterator_tag : bidirectional_iterator_tag
    {
    };

    struct continuous_iterator_tag : public random_access_iterator_tag
    {
    }; // Extension to the C++ standard. Contiguous ranges are more than random access, they are physically contiguous.

  } // namespace v1
} // namespace rsl
