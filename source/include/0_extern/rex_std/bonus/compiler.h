// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: compiler.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      enum class compiler
      {
        clang,
        msvc,
        gcc
      };

#ifdef __clang__
  #define REX_CLANG_COMPILER
      constexpr compiler g_compiler = compiler::clang;
#elif _MSC_VER
  #define REX_MSVC_COMPILER
      constexpr compiler g_compiler = compiler::msvc;
#elif __GNUC__
  #define REX_GCC_COMPILER
      constexpr compiler g_compiler = compiler::gcc;
#else
  #error unknown compiler used
#endif
    } // namespace internal
  }   // namespace v1
} // namespace rsl