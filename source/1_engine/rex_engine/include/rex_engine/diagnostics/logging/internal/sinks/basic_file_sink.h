

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/file_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
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
      explicit BasicFileSink(const filename_t& filename, bool truncate = false, const FileEventHandlers& eventHandlers = {});
      const filename_t& filename() const;

    protected:
      void sink_it_impl(const details::LogMsg& msg) override;
      void flush_impl() override;

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
  inline rsl::shared_ptr<Logger> basic_logger_mt(const rex::DebugString& loggerName, const filename_t& filename, bool truncate = false, const FileEventHandlers& eventHandlers = {})
  {
    return Factory::template create<sinks::basic_file_sink_mt>(logger_name, filename, truncate, event_handlers);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> basic_logger_st(const rex::DebugString& loggerName, const filename_t& filename, bool truncate = false, const FileEventHandlers& eventHandlers = {})
  {
    return Factory::template create<sinks::basic_file_sink_st>(logger_name, filename, truncate, event_handlers);
  }

} // namespace rexlog