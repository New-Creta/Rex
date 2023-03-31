#pragma once

#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"

namespace rex
{
  class FrameInfo
  {
  public:
    FrameInfo();

    void update();

    const DeltaTime& delta_time() const;
    const FPS& fps() const;
    card32 index() const;

  private:
    DeltaTime m_delta_time;
    FPS m_fps;
    card32 m_index;
  };
} // namespace rex