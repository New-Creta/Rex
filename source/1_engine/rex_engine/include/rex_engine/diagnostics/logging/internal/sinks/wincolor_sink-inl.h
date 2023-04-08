

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/windows_include.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/wincolor_sink.h"

#include <wincon.h>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  namespace sinks
  {
    template <typename ConsoleMutex>
    REXLOG_INLINE WincolorSink<ConsoleMutex>::WincolorSink(void* outHandle, ColorMode mode)
        : m_out_handle(outHandle)
        , m_mutex(&ConsoleMutex::mutex())
        , m_formatter(details::make_unique<rexlog::PatternFormatter>())
    {
      set_color_mode_impl(mode);
      // set level colors
      m_colors[level::Trace]    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;                                         // white
      m_colors[level::Debug]    = FOREGROUND_GREEN | FOREGROUND_BLUE;                                                          // cyan
      m_colors[level::Info]     = FOREGROUND_GREEN;                                                                            // green
      m_colors[level::Warn]     = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;                                    // intense yellow
      m_colors[level::Err]      = FOREGROUND_RED | FOREGROUND_INTENSITY;                                                       // intense red
      m_colors[level::Critical] = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // intense white on red background
      m_colors[level::Off]      = 0;
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE WincolorSink<ConsoleMutex>::~WincolorSink()
    {
      this->flush();
    }

    // change the color for the given level
    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::set_color(level::LevelEnum level, rsl::uint16 color)
    {
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      m_colors[static_cast<count_t>(level)] = color;
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::log(const details::LogMsg& msg)
    {
      if(m_out_handle == nullptr || m_out_handle == INVALID_HANDLE_VALUE)
      {
        return;
      }

      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      msg.color_range_start = 0;
      msg.color_range_end   = 0;
      memory_buf_t formatted;
      m_formatter->format(msg, formatted);
      if(m_should_do_colors && msg.color_range_end > msg.color_range_start)
      {
        // before color range
        print_range_impl(formatted, 0, msg.color_range_start);
        // in color range
        auto orig_attribs = static_cast<WORD>(set_foreground_color_impl(m_colors[static_cast<count_t>(msg.level)]));
        print_range_impl(formatted, msg.color_range_start, msg.color_range_end);
        // reset to orig colors
        ::SetConsoleTextAttribute(static_cast<HANDLE>(m_out_handle), orig_attribs);
        print_range_impl(formatted, msg.color_range_end, formatted.size());
      }
      else // print without colors if color range is invalid (or color is disabled)
      {
        write_to_file_impl(formatted);
      }
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::flush()
    {
      // windows console always flushed?
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::set_pattern(const rex::DebugString& pattern)
    {
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      m_formatter = rsl::make_unique<PatternFormatter>(pattern);
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sinkFormatter)
    {
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      m_formatter = rsl::move(sinkFormatter);
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::set_color_mode(ColorMode mode)
    {
      const rsl::unique_lock<mutex_t> lock(*m_mutex);
      set_color_mode_impl(mode);
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::set_color_mode_impl(ColorMode mode)
    {
      if(mode == ColorMode::Automatic)
      {
        // should do colors only if out_handle_  points to actual console.
        DWORD console_mode    = 0;
        const bool in_console = ::GetConsoleMode(static_cast<HANDLE>(m_out_handle), &console_mode) != 0;
        m_should_do_colors    = in_console;
      }
      else
      {
        m_should_do_colors = mode == ColorMode::Always;
      }
    }

    // set foreground color and return the orig console attributes (for resetting later)
    template <typename ConsoleMutex>
    rsl::uint16 REXLOG_INLINE WincolorSink<ConsoleMutex>::set_foreground_color_impl(rsl::uint16 attribs)
    {
      CONSOLE_SCREEN_BUFFER_INFO orig_buffer_info;
      if(::GetConsoleScreenBufferInfo(static_cast<HANDLE>(m_out_handle), &orig_buffer_info) == 0)
      {
        // just return white if failed getting console info
        return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
      }

      // change only the foreground bits (lowest 4 bits)
      auto new_attribs = static_cast<WORD>(attribs) | (orig_buffer_info.wAttributes & 0xfff0); // NOLINT(hicpp-signed-bitwise)
      auto ignored     = ::SetConsoleTextAttribute(static_cast<HANDLE>(m_out_handle), static_cast<WORD>(new_attribs));
      (void)(ignored);
      return static_cast<rsl::uint16>(orig_buffer_info.wAttributes); // return orig attribs
    }

    // print a range of formatted message to console
    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::print_range_impl(const memory_buf_t& formatted, size_t start, size_t end)
    {
      if(end > start)
      {
        auto size    = static_cast<DWORD>(end - start);
        auto ignored = ::WriteConsoleA(static_cast<HANDLE>(m_out_handle), formatted.data() + start, size, nullptr, nullptr);
        (void)(ignored);
      }
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE WincolorSink<ConsoleMutex>::write_to_file_impl(const memory_buf_t& formatted)
    {
      auto size           = static_cast<DWORD>(formatted.size());
      DWORD bytes_written = 0;
      auto ignored        = ::WriteFile(static_cast<HANDLE>(m_out_handle), formatted.data(), size, &bytes_written, nullptr);
      (void)(ignored);
    }

    // wincolor_stdout_sink
    template <typename ConsoleMutex>
    REXLOG_INLINE WincolorStdoutSink<ConsoleMutex>::WincolorStdoutSink(ColorMode mode)
        : WincolorSink<ConsoleMutex>(::GetStdHandle(STD_OUTPUT_HANDLE), mode)
    {
    }

    // wincolor_stderr_sink
    template <typename ConsoleMutex>
    REXLOG_INLINE WincolorStderrSink<ConsoleMutex>::WincolorStderrSink(ColorMode mode)
        : WincolorSink<ConsoleMutex>(::GetStdHandle(STD_ERROR_HANDLE), mode)
    {
    }
  } // namespace sinks
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)

