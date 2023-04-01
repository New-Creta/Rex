

#pragma once

#include "rex_std/memory.h"

#include <array>
#include <cstdint>
#include <mutex>
#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/details/console_globals.h>
#include <rex_engine/diagnostics/logging/internal/details/null_mutex.h>
#include <rex_engine/diagnostics/logging/internal/sinks/sink.h>
#include <string>

namespace rexlog
{
  namespace sinks
  {
    /*
     * Windows color console sink. Uses WriteConsoleA to write to the console with
     * colors
     */
    template <typename ConsoleMutex>
    class wincolor_sink : public sink
    {
    public:
      wincolor_sink(void* out_handle, color_mode mode);
      ~wincolor_sink() override;

      wincolor_sink(const wincolor_sink& other)            = delete;
      wincolor_sink& operator=(const wincolor_sink& other) = delete;

      // change the color for the given level
      void set_color(level::level_enum level, rsl::uint16 color);
      void log(const details::log_msg& msg) final override;
      void flush() final override;
      void set_pattern(const rex::DebugString& pattern) override final;
      void set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter) override final;
      void set_color_mode(color_mode mode);

    protected:
      using mutex_t = typename ConsoleMutex::mutex_t;
      void* out_handle_;
      mutex_t& mutex_;
      bool should_do_colors_;
      rsl::unique_ptr<rexlog::formatter> formatter_;
      rsl::array<rsl::uint16, level::n_levels> colors_;

      // set foreground color and return the orig console attributes (for resetting later)
      rsl::uint16 set_foreground_color_(rsl::uint16 attribs);

      // print a range of formatted message to console
      void print_range_(const memory_buf_t& formatted, size_t start, size_t end);

      // in case we are redirected to file (not in console mode)
      void write_to_file_(const memory_buf_t& formatted);

      void set_color_mode_impl(color_mode mode);
    };

    template <typename ConsoleMutex>
    class wincolor_stdout_sink : public wincolor_sink<ConsoleMutex>
    {
    public:
      explicit wincolor_stdout_sink(color_mode mode = color_mode::automatic);
    };

    template <typename ConsoleMutex>
    class wincolor_stderr_sink : public wincolor_sink<ConsoleMutex>
    {
    public:
      explicit wincolor_stderr_sink(color_mode mode = color_mode::automatic);
    };

    using wincolor_stdout_sink_mt = wincolor_stdout_sink<details::console_mutex>;
    using wincolor_stdout_sink_st = wincolor_stdout_sink<details::console_nullmutex>;

    using wincolor_stderr_sink_mt = wincolor_stderr_sink<details::console_mutex>;
    using wincolor_stderr_sink_st = wincolor_stderr_sink<details::console_nullmutex>;
  } // namespace sinks
} // namespace rexlog