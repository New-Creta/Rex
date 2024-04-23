#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  template <typename T>
  class ObjectCounter
  {
  public:
    ObjectCounter()
        : m_instance_nr(s_instance_count)
    {
      ++s_instance_count;
    }

    virtual ~ObjectCounter()
    {
      --s_instance_count;
    }

    static s32 instance_count()
    {
      return s_instance_count;
    }

    s32 instance_nr() const
    {
      return m_instance_nr;
    }

  private:
    static s32 s_instance_count;
    s32 m_instance_nr;
  };

  template <typename T>
  s32 ObjectCounter<T>::s_instance_count = 0;
} // namespace rex