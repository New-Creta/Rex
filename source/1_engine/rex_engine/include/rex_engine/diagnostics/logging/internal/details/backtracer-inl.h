

#pragma once

#include "rex_std/mutex.h"

#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"

namespace rexlog
{
  namespace details
  {
    REXLOG_INLINE inline Backtracer::Backtracer(const Backtracer& other) 
    {
      rsl::unique_lock<rsl::mutex> lock(other.m_mutex);
      m_enabled = other.enabled();
      m_messages = other.m_messages;
    }

    REXLOG_INLINE inline Backtracer::Backtracer(Backtracer&& other) REXLOG_NOEXCEPT 
    {
      rsl::unique_lock<rsl::mutex> lock(other.m_mutex);
      m_enabled = other.enabled();
      m_messages = rsl::move(other.m_messages);
    }

    Backtracer::~Backtracer() = default;

    REXLOG_INLINE inline Backtracer& Backtracer::operator=(const Backtracer& other)
    {
      rsl::unique_lock<rsl::mutex> const lock(m_mutex);
      m_enabled = other.enabled();
      m_messages = other.m_messages;
      return *this;
    }

    REXLOG_INLINE inline Backtracer& Backtracer::operator=(Backtracer&& other)
    {
      rsl::unique_lock<rsl::mutex> const lock(m_mutex);
      m_enabled  = other.enabled();
      m_messages = rsl::move(other.m_messages);
      return *this;
    }

    REXLOG_INLINE inline void Backtracer::enable(size_t size)
    {
      rsl::unique_lock<rsl::mutex> const lock {m_mutex};
      m_enabled.store(true, rsl::memory_order_relaxed);
      m_messages = CircularQ<LogMsgBuffer> {size};
    }

    REXLOG_INLINE inline void Backtracer::disable()
    {
      rsl::unique_lock<rsl::mutex> const lock {m_mutex};
      m_enabled.store(false, rsl::memory_order_relaxed);
    }

    REXLOG_INLINE inline bool Backtracer::enabled() const
    {
      return m_enabled.load(rsl::memory_order_relaxed);
    }

    REXLOG_INLINE inline void Backtracer::push_back(const LogMsg& msg)
    {
      rsl::unique_lock<rsl::mutex> const lock {m_mutex};
      m_messages.push_back(LogMsgBuffer {msg});
    }

    REXLOG_INLINE inline bool Backtracer::empty() const
    {
      rsl::unique_lock<rsl::mutex> const lock {m_mutex};
      return m_messages.empty();
    }

    // pop all items in the q and apply the given fun on each of them.
    REXLOG_INLINE inline void Backtracer::foreach_pop(const rsl::function<void(const details::LogMsg&)>& fun)
    {
      rsl::unique_lock<rsl::mutex> const lock {m_mutex};
      while(!m_messages.empty())
      {
        auto& front_msg = m_messages.front();
        fun(front_msg);
        m_messages.pop_front();
      }
    }
  } // namespace details
} // namespace rexlog
