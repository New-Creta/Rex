#pragma once

#include "rex_std/assert.h"

namespace rex
{
  void rex_assert(const rsl::fmt_stack_string& msg);
}

#define REX_ASSERT(...)         RSL_ASSERT(__VA_ARGS__)
#define REX_ASSERT_X(cond, ...) RSL_ASSERT_X(cond, __VA_ARGS__)