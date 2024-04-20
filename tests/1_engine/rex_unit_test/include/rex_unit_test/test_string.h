#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/string.h"

namespace rex::test
{
  void create_random_string(rsl::string& str, s32 length = 10);
}