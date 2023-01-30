#pragma once

#include "rex_engine/types.h"

namespace rex
{
  struct ApplicationCreationParams;

  extern ApplicationCreationParams app_entry(s32 argc, c8** argv);
}