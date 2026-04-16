#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/type_traits.h"

namespace rex
{
  // A state controller is just a wrapper around a bool 
  // allowing for easier bit manipulation of the enum
  // to add or remove states, or test for presence of a flag
  template <typename TState, rsl::enable_if_t<rsl::is_enum_v<TState>, bool> = true>
  class StateController
  {
  public:
    //-----------------------------------------------------------------------
    StateController()
        : m_state(static_cast<TState>(0))
    {
    }
    //-----------------------------------------------------------------------
    explicit StateController(TState state)
        : m_state(state)
    {
    }

    //-----------------------------------------------------------------------
    void add_state(TState state)
    {
      m_state = static_cast<TState>(static_cast<u32>(m_state) | static_cast<u32>(state));
    }
    //-----------------------------------------------------------------------
    void remove_state(TState state)
    {
      m_state = static_cast<TState>(static_cast<u32>(m_state) & ~static_cast<u32>(state));
    }
    //-----------------------------------------------------------------------
    void change_state(TState state)
    {
      m_state = static_cast<TState>(0);

      add_state(state);
    }

    //-----------------------------------------------------------------------
    bool has_state(TState state) const
    {
      return (static_cast<u32>(m_state) & static_cast<u32>(state)) != 0u;
    }
    //-----------------------------------------------------------------------
    TState state() const
    {
      return m_state;
    }

  private:
    TState m_state;
  };
} // namespace rex