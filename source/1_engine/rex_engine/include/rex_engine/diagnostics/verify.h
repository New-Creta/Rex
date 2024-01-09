#pragma once

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  // This is very similar to a rex_assert message but is enabled in release as well
  void rex_verify(rsl::string_view condition, rsl::string_view msg);
} // namespace rex

#define REX_VERIFY(...)                                                                                                                                                                                                                                  \
  rex::rex_verify("forced verify", rsl::format(__VA_ARGS__));            \
  while(true)                                           \
  {                                                     \
    DEBUG_BREAK();                                      \
  }

#define REX_VERIFY_X(cond, ...)                         \
  if(!(cond))                                           \
  {                                                     \
    REX_VERIFY(#cond, __VA_ARGS__);                            \
  }