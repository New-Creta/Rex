#pragma once

#include "rex_engine/timing/interval.h"

#include "rex_std/chrono.h"

using namespace rsl::chrono_literals;

namespace rex
{
    class FPS
    {
    public:

        //--------------------------------------------------------------------------------------------
        FPS()
            :m_fps(0)
            ,m_fps_count(0)
        {}

        //--------------------------------------------------------------------------------------------
        void update()
        {
            ++m_fps_count;

            if (m_fps_interval.value() > rsl::chrono::duration_cast<rsl::chrono::milliseconds>(1s).count())
            {
                m_fps = m_fps_count;

                m_fps_count = 0;
                m_fps_interval = Interval();
            }
        }

        //--------------------------------------------------------------------------------------------
        int get() const
        {
            return m_fps;
        }

    private:
        int m_fps;
        int m_fps_count;

        Interval m_fps_interval;
    };
}