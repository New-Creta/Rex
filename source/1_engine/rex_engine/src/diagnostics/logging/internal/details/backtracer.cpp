#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"
#include "rex_std/mutex.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  namespace details
  {
     Backtracer::Backtracer(const Backtracer& other)
        : m_enabled(false)
        , m_messages()
    {
      const rsl::unique_lock<rsl::mutex> lock(other.m_mutex);
      m_enabled  = other.enabled();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
      m_messages = other.m_messages; // NOLINT(cppcoreguidelines-prefer-member-initializer)
    }

     Backtracer::Backtracer(Backtracer&& other) REXLOG_NOEXCEPT : m_enabled(false), m_messages()
    {
      const rsl::unique_lock<rsl::mutex> lock(other.m_mutex);
      m_enabled  = other.enabled();             // NOLINT(cppcoreguidelines-prefer-member-initializer)
      m_messages = rsl::move(other.m_messages); // NOLINT(cppcoreguidelines-prefer-member-initializer)
    }

     Backtracer::~Backtracer() = default;

     Backtracer& Backtracer::operator=(const Backtracer& other)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_mutex);
      m_enabled  = other.enabled();
      m_messages = other.m_messages;
      return *this;
    }

     Backtracer& Backtracer::operator=(Backtracer&& other)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_mutex);
      m_enabled  = other.enabled();
      m_messages = rsl::move(other.m_messages);
      return *this;
    }

     void Backtracer::enable(size_t size)
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      m_enabled.store(true, rsl::memory_order_relaxed);
      m_messages = CircularQ<LogMsgBuffer> {size};
    }

     void Backtracer::disable()
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      m_enabled.store(false, rsl::memory_order_relaxed);
    }

     bool Backtracer::enabled() const
    {
      return m_enabled.load(rsl::memory_order_relaxed);
    }

     void Backtracer::push_back(const LogMsg& msg)
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      m_messages.push_back(LogMsgBuffer {msg});
    }

     bool Backtracer::empty() const
    {
      const rsl::unique_lock<rsl::mutex> lock {m_mutex};
      return m_messages.empty();
    }

    // pop all items in the q and apply the given fun on each of them.
     void Backtracer::foreach_pop(const rsl::function<void(const details::LogMsg&)>& fun)
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
