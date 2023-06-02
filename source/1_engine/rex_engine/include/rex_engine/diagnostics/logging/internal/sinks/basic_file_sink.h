

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/file_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

#include <mutex>
#include <string>

namespace rexlog
{
  namespace sinks
  {
    /*
     * Trivial file sink with single file as target
     */
    template <typename Mutex>
    class BasicFileSink final : public BaseSink<Mutex>
    {
    public:
      explicit BasicFileSink(rsl::string_view filename, bool truncate = false, const FileEventHandlers& eventHandlers = {});
      rsl::string_view filename() const;

    protected:
      void sink_it_impl(const details::LogMsg& msg) override;
      void flush_it_impl() override;

    private:
      details::FileHelper m_file_helper;
    };

    template <typename Mutex>
    BasicFileSink<Mutex>::BasicFileSink(rsl::string_view filename, bool truncate, const FileEventHandlers& eventHandlers)
        : m_file_helper {eventHandlers}
    {
      m_file_helper.open(filename, truncate);
    }

    template <typename Mutex>
    rsl::string_view BasicFileSink<Mutex>::filename() const
    {
      return m_file_helper.filename();
    }

    template <typename Mutex>
    void BasicFileSink<Mutex>::sink_it_impl(const details::LogMsg& msg)
    {
      memory_buf_t formatted;
      BaseSink<Mutex>::formatter()->format(msg, formatted);
      m_file_helper.write(formatted);
    }

    template <typename Mutex>
    void BasicFileSink<Mutex>::flush_it_impl()
    {
      m_file_helper.flush();
    }

    using basic_file_sink_mt = BasicFileSink<rsl::mutex>;
    using basic_file_sink_st = BasicFileSink<details::NullMutex>;

  } // namespace sinks
} // namespace rexlog