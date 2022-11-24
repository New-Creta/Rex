// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_severity.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  enum class Severity
  {
    Info,       // Info messages
    Warning,    // Warning messages, plausible behavior but possible bugs
    Error,      // Error messages, bugs, invalid code paths or data, but game can resolve them
    Fatal       // Fatal messages, this is really bad, game can't resolve and will crash
  };
}