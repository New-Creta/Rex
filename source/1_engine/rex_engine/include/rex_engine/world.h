#pragma once

#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"

namespace rex
{
  class World
  {
  public:
    static const DeltaTime& get_delta_time();
    static const FPS& get_frames_per_seconds();

    World();
    ~World();

    void update();

  private:
    static DeltaTime s_delta_time;
    static FPS s_frames_per_second;
  };
} // namespace rex