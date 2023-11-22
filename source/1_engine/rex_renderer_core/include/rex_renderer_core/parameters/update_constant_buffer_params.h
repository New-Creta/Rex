#pragma once

#include "rex_engine/state_controller.h"
#include "rex_engine/types.h"
#include "rex_renderer_core/clear_bits.h"
#include "rex_std_extra/math/color.h"

namespace rex
{
  namespace renderer
  {
    namespace parameters
    {
      struct UpdateConstantBuffer
      {
        UpdateConstantBuffer()
            : data(nullptr)
            , data_size(0)
            , offset(0)
        {
        }

        void*   data;
        s32     data_size;
        s32     offset;
      };
    } // namespace parameters
  }   // namespace renderer
} // namespace rex