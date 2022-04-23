#include "rex_engine_pch.h"

#include "world.h"

namespace rex
{
    //-------------------------------------------------------------------------
    DeltaTime  World::s_delta_time;
    FPS        World::s_frames_per_second;

    //-------------------------------------------------------------------------
    const DeltaTime& World::getDeltaTime()
    {
        return World::s_delta_time;
    }
    //-------------------------------------------------------------------------
    const FPS& World::getFramesPerSecond()
    {
        return World::s_frames_per_second;
    }

    //-------------------------------------------------------------------------
    World::World() = default;
    //-------------------------------------------------------------------------
    World::~World() = default;

    //-------------------------------------------------------------------------
    void World::update()
    {
        World::s_delta_time.update();
        World::s_frames_per_second.update();
    }
}