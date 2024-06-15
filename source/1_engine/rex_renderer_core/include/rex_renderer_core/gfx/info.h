#pragma once

#include "rex_std/bonus/string.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace gfx
  {
    // Some info about the rendering infrastructure
    struct Info
    {
      rsl::small_stack_string api;              // eg: DirectX
      rsl::small_stack_string api_version;      // eg: D3D_FEATURE_LEVEL_12_0
      rsl::small_stack_string shader_version;   // eg: D3D_SHADER_MODEL_6_5
      rsl::small_stack_string adaptor;          // eg: NVIDIA GeForce RTX 3060 Ti
      rsl::small_stack_string vendor;           // eg: NVIDIA
      rsl::small_stack_string driver_version;   // eg: 552.85
      rsl::memory_size available_memory;        // eg: 8 388 608
    };
  }
}