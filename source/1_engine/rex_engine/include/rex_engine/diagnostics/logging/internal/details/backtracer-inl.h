

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"
#include "rex_std/mutex.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  namespace details
  {
    REXLOG_INLINE Backtracer::Backtracer(const Backtracer& other)
        : m_enabled(false)
        , m_messages()
    {
      const rsl::unique_lock<rsl::mutex> lock(other.m_mutex);
      m_enabled  = other.enabled();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
      m_messages = other.m_messages; // NOLINT(cppcoreguidelines-prefer-member-initializer)
    }

    REXLOG_INLINE Backtracer::Backtracer(Backtracer&& other) REXLOG_NOEXCEPT : m_enabled(false), m_messages()
    {
      const rsl::unique_lock<rsl::mutex> lock(other.m_mutex);
      m_enabled  = other.enabled();             // NOLINT(cppcoreguidelines-prefer-member-initializer)
      m_messages = rsl::move(other.m_messages); // NOLINT(cppcoreguidelines-prefer-member-initializer)
    }

    REXLOG_INLINE Backtracer::~Backtracer() = default;

    REXLOG_INLINE Backtracer& Backtracer::operator=(const Backtracer& other)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_mutex);
      m_enabled  = other.enabled();
      m_messages = other.m_messages;
      return *this;
    }

    REXLOG_INLINE Backtracer& Backtracer::operator=(Backtracer&& other)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_mutex);
      m_enabled  = other.enabled();
      m_messages = rsl::move(other.m_messages);
      return *this;
    }

    REXLOG_INLINE void Backtracer::enable(size_t size)
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      m_enabled.store(true, rsl::memory_order_relaxed);
      m_messages = CircularQ<LogMsgBuffer> {size};
    }

    REXLOG_INLINE void Backtracer::disable()
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      m_enabled.store(false, rsl::memory_order_relaxed);
    }

    REXLOG_INLINE bool Backtracer::enabled() const
    {
      return m_enabled.load(rsl::memory_order_relaxed);
    }

    REXLOG_INLINE void Backtracer::push_back(const LogMsg& msg)
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      m_messages.push_back(LogMsgBuffer {msg});
    }

    REXLOG_INLINE bool Backtracer::empty() const
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      return m_messages.empty();
    }

    // pop all items in the q and apply the given fun on each of them.
    REXLOG_INLINE void Backtracer::foreach_pop(const rsl::function<void(const details::LogMsg&)>& fun)
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      while(!m_messages.empty())
      {
        auto& front_msg = m_messages.front();
        fun(front_msg);
        m_messages.pop_front();
      }
    }
  } // namespace details
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
