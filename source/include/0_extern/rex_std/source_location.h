// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: source_location.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/header/source_location
//
// Represents information about source code.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/types.h"

#ifdef REX_ENABLE_WITH_CPP20
  #define REX_CONSTEVAL_WITH_CPP20 consteval
#else
  #define REX_CONSTEVAL_WITH_CPP20 constexpr
#endif

namespace rsl
{
  inline namespace v1
  {

    class source_location
    {
    public:
      REX_CONSTEVAL_WITH_CPP20 source_location()
          : m_line(0)
          , m_column(0)
          , m_file_name(nullptr)
          , m_function_name(nullptr)
      {
      }

      static constexpr source_location current(const card32 line = __builtin_LINE(), card32 column = __builtin_COLUMN(), const char8* file = __builtin_FILE(), const char8* function = __builtin_FUNCTION())
      {
        source_location res {};

        res.m_line          = line;
        res.m_column        = column;
        res.m_file_name     = file;
        res.m_function_name = function;

        return res;
      }

      card32 line() const
      {
        return m_line;
      }
      card32 column() const
      {
        return m_column;
      }
      const char8* file_name() const
      {
        return m_file_name;
      }
      const char8* function_name() const
      {
        return m_function_name;
      }

    private:
      card32 m_line;
      card32 m_column;
      const char8* m_file_name;
      const char8* m_function_name;
    };

  } // namespace v1
} // namespace rsl
