#pragma once

#include "rex_std/chrono.h"

namespace rex
{
  using fseconds     = rsl::chrono::duration<float>;
  using fmiliseconds = rsl::chrono::duration<float, rsl::milli>;

  class DeltaTime
  {
  public:
    //-------------------------------------------------------------------------
    DeltaTime()
        : m_last_time_point(rsl::chrono::steady_clock::now())
        , m_current_time_point(rsl::chrono::steady_clock::now() - m_last_time_point)
    {
    }

    //-------------------------------------------------------------------------
    void update()
    {
      const rsl::chrono::steady_clock::time_point current = rsl::chrono::steady_clock::now();

      m_current_time_point = fmiliseconds(current - m_last_time_point);
      m_last_time_point    = current;
    }

    //--------------------------------------------------------------------------------------------
    float to_seconds() const
    {
      return fseconds {m_current_time_point}.count();
    }
    //--------------------------------------------------------------------------------------------
    float to_milliseconds() const
    {
      return m_current_time_point.count();
    }

  private:
    rsl::chrono::steady_clock::time_point m_last_time_point;
    fmiliseconds m_current_time_point;
  };
} // namespace rex