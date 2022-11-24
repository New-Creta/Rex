// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: stack_string.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/string/stack_string.h"

rsl::tiny_stack_string rsl::to_stack_string(const uint32 value)
{
  return internal::to_string<tiny_stack_string>(value);
}
rsl::tiny_stack_string rsl::to_stack_string(const int32 value)
{
  return internal::to_string<tiny_stack_string>(value);
}
rsl::tiny_stack_string rsl::to_stack_string(const uint64 value)
{
  return internal::to_string<tiny_stack_string>(value);
}
rsl::tiny_stack_string rsl::to_stack_string(const int64 value)
{
  return internal::to_string<tiny_stack_string>(value);
}
rsl::tiny_stack_string rsl::to_stack_string(const float32 value, card32 precision)
{
  return internal::to_string<tiny_stack_string>(value, precision);
}
rsl::tiny_stack_string rsl::to_stack_string(const float64 value, card32 precision)
{
  return internal::to_string<tiny_stack_string>(value, precision);
}
rsl::tiny_stack_string rsl::to_stack_string(const void* ptr)
{
  return internal::to_string<tiny_stack_string>(ptr);
}

rsl::tiny_stack_string operator"" _tiny(const char* str, size_t length)
{
  return rsl::tiny_stack_string(str, static_cast<card32>(length + 1));
}
rsl::small_stack_string operator"" _small(const char* str, size_t length)
{
  return rsl::small_stack_string(str, static_cast<card32>(length + 1));
}
rsl::medium_stack_string operator"" _med(const char* str, size_t length)
{
  return rsl::medium_stack_string(str, static_cast<card32>(length + 1));
}
rsl::big_stack_string operator"" _big(const char* str, size_t length)
{
  return rsl::big_stack_string(str, static_cast<card32>(length + 1));
}