

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/console_globals.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_std/memory.h"
#include "rex_std_extra/utility/enum_reflection.h"

#include <array>
#include <cstdint>
#include <mutex>
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
    class WincolorSink : public sink
    {
    public:
      WincolorSink(void* outHandle, ColorMode mode);
      ~WincolorSink() override;

      WincolorSink(const WincolorSink& other)            = delete;
      WincolorSink& operator=(const WincolorSink& other) = delete;

      // change the color for the given level
      void set_color(level::LevelEnum level, rsl::uint16 color);
      void log(const details::LogMsg& msg) final;
      void flush() final;
      void set_pattern(const rex::DebugString& pattern) final;
      void set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter) final;
      void set_color_mode(ColorMode mode);

    protected:
      using mutex_t = typename ConsoleMutex::mutex_t;

      // set foreground color and return the orig console attributes (for resetting later)
      rsl::uint16 set_foreground_color_impl(rsl::uint16 attribs);

      // print a range of formatted message to console
      void print_range_impl(const memory_buf_t& formatted, size_t start, size_t end);

      // in case we are redirected to file (not in console mode)
      void write_to_file_impl(const memory_buf_t& formatted);

      void set_color_mode_impl(ColorMode mode);

    private:
      void* m_out_handle;
      mutex_t* m_mutex;
      bool m_should_do_colors {};
      rsl::unique_ptr<rexlog::formatter> m_formatter;
      rsl::array<rsl::uint16, rsl::enum_refl::enum_count<level::LevelEnum>()> m_colors;
    };

    template <typename ConsoleMutex>
    class WincolorStdoutSink : public WincolorSink<ConsoleMutex>
    {
    public:
      explicit WincolorStdoutSink(ColorMode mode = ColorMode::Automatic);

    private:
        void* sink_handle();
    };

    template <typename ConsoleMutex>
    class WincolorStderrSink : public WincolorSink<ConsoleMutex>
    {
    public:
      explicit WincolorStderrSink(ColorMode mode = ColorMode::Automatic);

    private:
        void* sink_handle();
    };

    // wincolor_stdout_sink
    template <typename ConsoleMutex>
     WincolorStdoutSink<ConsoleMutex>::WincolorStdoutSink(ColorMode mode)
        : WincolorSink<ConsoleMutex>(sink_handle(), mode)
    {
    }

    // wincolor_stderr_sink
    template <typename ConsoleMutex>
     WincolorStderrSink<ConsoleMutex>::WincolorStderrSink(ColorMode mode)
        : WincolorSink<ConsoleMutex>(sink_handle(), mode)
    {
    }

    using wincolor_stdout_sink_mt = WincolorStdoutSink<details::ConsoleMutex>;
    using wincolor_stdout_sink_st = WincolorStdoutSink<details::ConsoleNullMutex>;

    using wincolor_stderr_sink_mt = WincolorStderrSink<details::ConsoleMutex>;
    using wincolor_stderr_sink_st = WincolorStderrSink<details::ConsoleNullMutex>;
  } // namespace sinks
} // namespace rexlog