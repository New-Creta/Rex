#pragma once

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  void rex_verify(const rsl::fmt_stack_string& msg);
} // namespace rex

#define REX_VERIFY(...)                                                                                                                                                                                                                                  \
  rex::rex_verify(rsl::format(__VA_ARGS__));                                                                                                                                                                                                             \
  while(true)                                                                                                                                                                                                                                            \
  {                                                                                                                                                                                                                                                      \
    DEBUG_BREAK();                                                                                                                                                                                                                                       \
  }
#define REX_VERIFY_X(cond, ...)                                                                                                                                                                                                                          \
  if(!(cond))                                                                                                                                                                                                                                            \
  {                                                                                                                                                                                                                                                      \
    REX_VERIFY(__VA_ARGS__);                                                                                                                                                                                                                             \
  }