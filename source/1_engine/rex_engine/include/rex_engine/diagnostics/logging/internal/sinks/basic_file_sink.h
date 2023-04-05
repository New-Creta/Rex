

#pragma once

#include <mutex>
#include "rex_engine/diagnostics/logging/internal/details/file_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/SynchronousFactory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
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
      explicit BasicFileSink(const filename_t& filename, bool truncate = false, const file_event_handlers& event_handlers = {});
      const filename_t& filename() const;

    protected:
      void sink_it_(const details::LogMsg& msg) override;
      void flush_() override;

    private:
      details::FileHelper m_file_helper;
    };

    using basic_file_sink_mt = BasicFileSink<rsl::mutex>;
    using basic_file_sink_st = BasicFileSink<details::NullMutex>;

  } // namespace sinks

  //
  // factory functions
  //
  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<logger> basic_logger_mt(const rex::DebugString& logger_name, const filename_t& filename, bool truncate = false, const file_event_handlers& event_handlers = {})
  {
    return Factory::template create<sinks::basic_file_sink_mt>(logger_name, filename, truncate, event_handlers);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<logger> basic_logger_st(const rex::DebugString& logger_name, const filename_t& filename, bool truncate = false, const file_event_handlers& event_handlers = {})
  {
    return Factory::template create<sinks::basic_file_sink_st>(logger_name, filename, truncate, event_handlers);
  }

} // namespace rexlog