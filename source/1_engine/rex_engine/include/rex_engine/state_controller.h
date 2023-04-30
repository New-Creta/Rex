#pragma once

#include "rex_engine/types.h"

#include <rex_std/type_traits.h>

namespace rex
{
  template <typename TState, rsl::enable_if_t<rsl::is_enum_v<TState>, bool> = true>
  class StateController
  {
  public:
    StateController()
        : m_state(0)
    {
    }
    explicit StateController(TState state)
        : m_state(static_cast<u32>(state))
    {
    }

    void add_state(TState state)
    {
      m_state = m_state | static_cast<u32>(state);
    }
    void remove_state(TState state)
    {
      m_state = m_state & ~static_cast<u32>(state);
    }

    bool has_state(TState state) const
    {
      return (m_state & static_cast<u32>(state)) != 0u;
    }

  private:
    u32 m_state;
  };
} // namespace rex