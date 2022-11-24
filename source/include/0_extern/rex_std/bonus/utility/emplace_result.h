// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: emplace_result.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename Iterator>
    struct emplace_result
    {
      Iterator inserted_element;
      bool emplace_successful;
    };
    template <typename Iterator>
    using insert_result = emplace_result<Iterator>;

  } // namespace v1
} // namespace rsl
