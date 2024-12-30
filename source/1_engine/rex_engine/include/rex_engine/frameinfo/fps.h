#pragma once

#include "rex_engine/timing/interval.h"
#include "rex_std/chrono.h"
#include "rex_std/algorithm.h"

namespace rex
{
  class FPS
  {
  public:
    //--------------------------------------------------------------------------------------------
    FPS()
        : m_fps(1) // initialize to 1 in case we get to the update too quickly
        , m_fps_count(0)
        , m_fps_interval()
    {
    }

    //--------------------------------------------------------------------------------------------
    void update()
    {
      // if 1 second has elapsed, reset the timer and start over
      using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
      if(m_fps_interval.milliseconds() > rsl::chrono::duration_cast<rsl::chrono::milliseconds>(1s).count())
      {
        m_fps = m_fps_count;

        m_fps_count    = 0;
        m_fps_interval.reset();
      }

      ++m_fps_count;
    }

    //--------------------------------------------------------------------------------------------
    // Return the fps of the last full second recorded
    int get() const
    {
      return m_fps;
    }

  private:
    int m_fps;
    int m_fps_count;

    Interval m_fps_interval;
  };
} // namespace rex