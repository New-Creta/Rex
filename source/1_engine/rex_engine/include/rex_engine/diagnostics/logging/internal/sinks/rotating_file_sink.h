

#pragma once

#include <chrono>
#include <mutex>
#include <rex_engine/diagnostics/logging/internal/details/file_helper.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/details/synchronous_factory.h>
#include <rex_engine/diagnostics/logging/internal/sinks/base_sink.h>
#include <string>

namespace rexlog
{
  namespace sinks
  {

    //
    // Rotating file sink based on size
    //
    template <typename Mutex>
    class rotating_file_sink final : public base_sink<Mutex>
    {
    public:
      rotating_file_sink(filename_t base_filename, rsl::size_t max_size, rsl::size_t max_files, bool rotate_on_open = false, const file_event_handlers& event_handlers = {});
      static filename_t calc_filename(const filename_t& filename, rsl::size_t index);
      filename_t filename();

    protected:
      void sink_it_(const details::log_msg& msg) override;
      void flush_() override;

    private:
      // Rotate files:
      // log.txt -> log.1.txt
      // log.1.txt -> log.2.txt
      // log.2.txt -> log.3.txt
      // log.3.txt -> delete
      void rotate_();

      // delete the target if exists, and rename the src file  to target
      // return true on success, false otherwise.
      bool rename_file_(const filename_t& src_filename, const filename_t& target_filename);

      filename_t base_filename_;
      rsl::size_t max_size_;
      rsl::size_t max_files_;
      rsl::size_t current_size_;
      details::file_helper file_helper_;
    };

    using rotating_file_sink_mt = rotating_file_sink<rsl::mutex>;
    using rotating_file_sink_st = rotating_file_sink<details::null_mutex>;

  } // namespace sinks

  //
  // factory functions
  //

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> rotating_logger_mt(const rsl::string& logger_name, const filename_t& filename, size_t max_file_size, size_t max_files, bool rotate_on_open = false, const file_event_handlers& event_handlers = {})
  {
    return Factory::template create<sinks::rotating_file_sink_mt>(logger_name, filename, max_file_size, max_files, rotate_on_open, event_handlers);
  }

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> rotating_logger_st(const rsl::string& logger_name, const filename_t& filename, size_t max_file_size, size_t max_files, bool rotate_on_open = false, const file_event_handlers& event_handlers = {})
  {
    return Factory::template create<sinks::rotating_file_sink_st>(logger_name, filename, max_file_size, max_files, rotate_on_open, event_handlers);
  }
} // namespace rexlog