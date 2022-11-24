// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_info.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/assert/assert_info.h"

#include "rex_std/internal/utility/move.h"

rsl::AssertInfo::AssertInfo(Severity severity, AssertID assert_id, rsl::string&& msg, rsl::string_view file, rsl::string_view func_sig, card32 line_nr)
  : m_severity(severity)
  , m_assert_id(assert_id)
  , m_message(rsl::move(msg))
  , m_file(file)
  , m_func_sig(func_sig)
  , m_line_nr(line_nr)
{
}

rsl::Severity rsl::AssertInfo::severity() const
{
  return m_severity;
}
rsl::AssertID rsl::AssertInfo::assert_id() const
{
  return m_assert_id;
}
rsl::string_view rsl::AssertInfo::msg() const
{
  return m_message;
}
rsl::string_view rsl::AssertInfo::file() const
{
  return m_file;
}
rsl::string_view rsl::AssertInfo::function() const
{
  return m_func_sig;
}
