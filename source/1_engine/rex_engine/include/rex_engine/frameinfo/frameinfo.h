#pragma once

#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"

namespace rex
{
  struct FrameInfo
  {
  public:
    //--------------------------------------------------------------------------------------------
    FrameInfo(const DeltaTime& dTime, const FPS& framesPerSecond)
        : delta_time(dTime)
        , fps(framesPerSecond)
    {
    }

    DeltaTime delta_time;
    FPS fps;
  };
} // namespace rex