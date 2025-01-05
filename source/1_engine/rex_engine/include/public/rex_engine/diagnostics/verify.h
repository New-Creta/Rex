#pragma once

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  // This is very similar to a rex_assert message but is enabled in release as well
  void rex_verify(rsl::string_view condition, rsl::string_view msg);

  // this is a forced verify that'll always trigger
  void rex_verify(rsl::string_view msg);
} // namespace rex

#define REX_VERIFY_X(cnd, msg) rex::rex_verify(condition, msg)
#define REX_VERIFY(msg)        rex::rex_verify(msg)
