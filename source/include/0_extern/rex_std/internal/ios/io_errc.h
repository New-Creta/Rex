// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: io_errc.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/io_errc
//
// Defines the error codes reported by I/O stream.
//-----------------------------------------------------------------------------

#pragma once

namespace rsl
{
  inline namespace v1
  {

    enum class io_errc
    {
      stream = 1
    };

  } // namespace v1
} // namespace rsl