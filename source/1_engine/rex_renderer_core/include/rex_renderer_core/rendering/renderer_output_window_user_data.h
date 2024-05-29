#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    // Wrapper around some user data that's relevant for the renderer
    struct OutputWindowUserData
    {
      void* primary_display_handle;

      s32 window_width;
      s32 window_height;

      s32 refresh_rate;

      bool windowed;

      s32 max_frames_in_flight;
    };
  } // namespace renderer
} // namespace rex