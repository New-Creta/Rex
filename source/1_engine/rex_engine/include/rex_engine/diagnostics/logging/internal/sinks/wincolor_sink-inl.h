

#pragma once

#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/details/windows_include.h>
#include <rex_engine/diagnostics/logging/internal/pattern_formatter.h>
#include <rex_engine/diagnostics/logging/internal/sinks/wincolor_sink.h>
#include <wincon.h>

namespace rexlog
{
  namespace sinks
  {
    template <typename ConsoleMutex>
    REXLOG_INLINE wincolor_sink<ConsoleMutex>::wincolor_sink(void* out_handle, color_mode mode)
        : out_handle_(out_handle)
        , mutex_(ConsoleMutex::mutex())
        , formatter_(details::make_unique<rexlog::pattern_formatter>())
    {
      set_color_mode_impl(mode);
      // set level colors
      colors_[level::trace]    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;                                         // white
      colors_[level::debug]    = FOREGROUND_GREEN | FOREGROUND_BLUE;                                                          // cyan
      colors_[level::info]     = FOREGROUND_GREEN;                                                                            // green
      colors_[level::warn]     = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;                                    // intense yellow
      colors_[level::err]      = FOREGROUND_RED | FOREGROUND_INTENSITY;                                                       // intense red
      colors_[level::critical] = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // intense white on red background
      colors_[level::off]      = 0;
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE wincolor_sink<ConsoleMutex>::~wincolor_sink()
    {
      this->flush();
    }

    // change the color for the given level
    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::set_color(level::level_enum level, rsl::uint16 color)
    {
      rsl::unique_lock<mutex_t> lock(mutex_);
      colors_[static_cast<size_t>(level)] = color;
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::log(const details::log_msg& msg)
    {
      if(out_handle_ == nullptr || out_handle_ == INVALID_HANDLE_VALUE)
      {
        return;
      }

      rsl::unique_lock<mutex_t> lock(mutex_);
      msg.color_range_start = 0;
      msg.color_range_end   = 0;
      memory_buf_t formatted;
      formatter_->format(msg, formatted);
      if(should_do_colors_ && msg.color_range_end > msg.color_range_start)
      {
        // before color range
        print_range_(formatted, 0, msg.color_range_start);
        // in color range
        auto orig_attribs = static_cast<WORD>(set_foreground_color_(colors_[static_cast<size_t>(msg.level)]));
        print_range_(formatted, msg.color_range_start, msg.color_range_end);
        // reset to orig colors
        ::SetConsoleTextAttribute(static_cast<HANDLE>(out_handle_), orig_attribs);
        print_range_(formatted, msg.color_range_end, formatted.size());
      }
      else // print without colors if color range is invalid (or color is disabled)
      {
        write_to_file_(formatted);
      }
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::flush()
    {
      // windows console always flushed?
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::set_pattern(const rsl::string& pattern)
    {
      rsl::unique_lock<mutex_t> lock(mutex_);
      formatter_ = rsl::make_unique<pattern_formatter>(pattern);
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter)
    {
      rsl::unique_lock<mutex_t> lock(mutex_);
      formatter_ = rsl::move(sink_formatter);
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::set_color_mode(color_mode mode)
    {
      rsl::unique_lock<mutex_t> lock(mutex_);
      set_color_mode_impl(mode);
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::set_color_mode_impl(color_mode mode)
    {
      if(mode == color_mode::automatic)
      {
        // should do colors only if out_handle_  points to actual console.
        DWORD console_mode;
        bool in_console   = ::GetConsoleMode(static_cast<HANDLE>(out_handle_), &console_mode) != 0;
        should_do_colors_ = in_console;
      }
      else
      {
        should_do_colors_ = mode == color_mode::always ? true : false;
      }
    }

    // set foreground color and return the orig console attributes (for resetting later)
    template <typename ConsoleMutex>
    rsl::uint16 REXLOG_INLINE wincolor_sink<ConsoleMutex>::set_foreground_color_(rsl::uint16 attribs)
    {
      CONSOLE_SCREEN_BUFFER_INFO orig_buffer_info;
      if(!::GetConsoleScreenBufferInfo(static_cast<HANDLE>(out_handle_), &orig_buffer_info))
      {
        // just return white if failed getting console info
        return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
      }

      // change only the foreground bits (lowest 4 bits)
      auto new_attribs = static_cast<WORD>(attribs) | (orig_buffer_info.wAttributes & 0xfff0);
      auto ignored     = ::SetConsoleTextAttribute(static_cast<HANDLE>(out_handle_), static_cast<WORD>(new_attribs));
      (void)(ignored);
      return static_cast<rsl::uint16>(orig_buffer_info.wAttributes); // return orig attribs
    }

    // print a range of formatted message to console
    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::print_range_(const memory_buf_t& formatted, size_t start, size_t end)
    {
      if(end > start)
      {
        auto size    = static_cast<DWORD>(end - start);
        auto ignored = ::WriteConsoleA(static_cast<HANDLE>(out_handle_), formatted.data() + start, size, nullptr, nullptr);
        (void)(ignored);
      }
    }

    template <typename ConsoleMutex>
    void REXLOG_INLINE wincolor_sink<ConsoleMutex>::write_to_file_(const memory_buf_t& formatted)
    {
      auto size           = static_cast<DWORD>(formatted.size());
      DWORD bytes_written = 0;
      auto ignored        = ::WriteFile(static_cast<HANDLE>(out_handle_), formatted.data(), size, &bytes_written, nullptr);
      (void)(ignored);
    }

    // wincolor_stdout_sink
    template <typename ConsoleMutex>
    REXLOG_INLINE wincolor_stdout_sink<ConsoleMutex>::wincolor_stdout_sink(color_mode mode)
        : wincolor_sink<ConsoleMutex>(::GetStdHandle(STD_OUTPUT_HANDLE), mode)
    {
    }

    // wincolor_stderr_sink
    template <typename ConsoleMutex>
    REXLOG_INLINE wincolor_stderr_sink<ConsoleMutex>::wincolor_stderr_sink(color_mode mode)
        : wincolor_sink<ConsoleMutex>(::GetStdHandle(STD_ERROR_HANDLE), mode)
    {
    }
  } // namespace sinks
} // namespace rexlog