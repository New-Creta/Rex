#pragma once

#include "rex_engine/timing/tickcount.h"

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
    ~Interval() = default;

    //-------------------------------------------------------------------------
    inline int64_t value() const
    {
      return get_tick_count() - m_initial;
    }

  private:
    int64_t m_initial;
  };

} // namespace rex