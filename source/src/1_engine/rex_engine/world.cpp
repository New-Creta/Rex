#include "rex_engine_pch.h"

#include "world.h"

namespace rex
{
    //-------------------------------------------------------------------------
    DeltaTime World::s_delta_time;
    FPS World::s_frames_per_second;

    //-------------------------------------------------------------------------
    const DeltaTime& World::get_delta_time()
    {
        return World::s_delta_time;
    }
    //-------------------------------------------------------------------------
    const FPS& World::get_frames_per_second()
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