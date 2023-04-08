

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/file_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#include <chrono>
#include <mutex>
#include <string>

namespace rexlog
{
  namespace sinks
  {

    //
    // Rotating file sink based on size
    //
    template <typename Mutex>
    class RotatingFileSink final : public BaseSink<Mutex>
    {
    public:
      RotatingFileSink(filename_t baseFilename, rsl::size_t maxSize, rsl::size_t maxFiles, bool rotateOnOpen = false, const FileEventHandlers& eventHandlers = {});
      static filename_t calc_filename(const filename_t& filename, rsl::size_t index);
      filename_t filename();

    protected:
      void sink_it_impl(const details::LogMsg& msg) override;
      void flush_impl() override;

    private:
      // Rotate files:
      // log.txt -> log.1.txt
      // log.1.txt -> log.2.txt
      // log.2.txt -> log.3.txt
      // log.3.txt -> delete
      void rotate_impl();

      // delete the target if exists, and rename the src file  to target
      // return true on success, false otherwise.
      bool rename_file_impl(const filename_t& srcFilename, const filename_t& targetFilename);

      filename_t m_base_filename;
      rsl::size_t m_max_size;
      rsl::size_t m_max_files;
      rsl::size_t m_current_size;
      details::FileHelper m_file_helper;
    };

    using rotating_file_sink_mt = RotatingFileSink<rsl::mutex>;
    using rotating_file_sink_st = RotatingFileSink<details::NullMutex>;

  } // namespace sinks

  //
  // factory functions
  //

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> rotating_logger_mt(const rex::DebugString& loggerName, const filename_t& filename, size_t maxFileSize, size_t maxFiles, bool rotateOnOpen = false, const FileEventHandlers& eventHandlers = {})
  {
    return Factory::template create<sinks::rotating_file_sink_mt>(logger_name, filename, max_file_size, max_files, rotate_on_open, event_handlers);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> rotating_logger_st(const rex::DebugString& loggerName, const filename_t& filename, size_t maxFileSize, size_t maxFiles, bool rotateOnOpen = false, const FileEventHandlers& eventHandlers = {})
  {
    return Factory::template create<sinks::rotating_file_sink_st>(logger_name, filename, max_file_size, max_files, rotate_on_open, event_handlers);
  }
} // namespace rexlog