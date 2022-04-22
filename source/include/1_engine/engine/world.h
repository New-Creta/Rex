#pragma once

#include "frameinfo/deltatime.h"
#include "frameinfo/fps.h"

namespace rex
{
    class World
    {
    public:
        static const DeltaTime& getDeltaTime();
        static const FPS& getFramesPerSecond();

        World();
        ~World();

        void update();

    private:
        static DeltaTime   s_delta_time;
        static FPS         s_frames_per_second;
    };
}