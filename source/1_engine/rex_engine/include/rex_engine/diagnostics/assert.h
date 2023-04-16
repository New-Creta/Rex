#pragma once

#include "rex_std/assert.h"
#include "rex_std/format.h"

namespace rex
{
  void rex_assert(const rsl::fmt_stack_string& msg);
}

#define REX_ASSERT(...)         rex::rex_assert(rsl::format(__VA_ARGS__))
#define REX_ASSERT_X(cond, ...) if (!(cond)) { REX_ASSERT(__VA_ARGS__); }