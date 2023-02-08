#pragma once

#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"

namespace rex
{
  struct FrameInfo
  {
    //--------------------------------------------------------------------------------------------
    FrameInfo(const DeltaTime& dTime, const FPS& framesPerSecond)
        : delta_time(dTime)
        , fps(framesPerSecond)
    {
    }

    const DeltaTime delta_time;
    const FPS fps;
  };
} // namespace rex