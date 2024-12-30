#pragma once

#include "rex_engine/timing/tickcount.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  class Interval
  {
  public:
    //-------------------------------------------------------------------------
    Interval()
        : m_initial(get_tick_count())
    {
    }

    //-------------------------------------------------------------------------
    // returns time in milliseconds
    int64_t milliseconds() const
    {
      return get_tick_count() - m_initial;
    }
    f32 seconds() const
    {
      return milliseconds() / static_cast<f32>(rsl::milli::den);
    }

    void reset()
    {
      m_initial = get_tick_count();
    }

  private:
    int64_t m_initial;
  };

} // namespace rex