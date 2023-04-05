

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/sinks/ansicolor_sink.h"

namespace rexlog
{
  namespace sinks
  {

    template <typename ConsoleMutex>
    REXLOG_INLINE ansicolor_sink<ConsoleMutex>::ansicolor_sink(FILE* target_file, color_mode mode)
        : m_target_file(target_file)
        , m_mutex(ConsoleMutex::mutex())
        , m_formatter(details::make_unique<rexlog::pattern_formatter>())

    {
      set_color_mode(mode);
      m_colors[level::trace]    = to_string_impl(white);
      m_colors[level::debug]    = to_string_impl(cyan);
      m_colors[level::info]     = to_string_impl(green);
      m_colors[level::warn]     = to_string_impl(yellow_bold);
      m_colors[level::err]      = to_string_impl(red_bold);
      m_colors[level::critical] = to_string_impl(bold_on_red);
      m_colors[level::off]      = to_string_impl(reset);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_color(level::level_enum color_level, string_view_t color)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_colors[static_cast<size_t>(color_level)] = to_string_impl(color);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::log(const details::LogMsg& msg)
    {
      // Wrap the originally formatted message in color codes.
      // If color is not supported in the terminal, log as is instead.
      rsl::lock_guard<mutex_t> lock(m_mutex);
      msg.color_range_start = 0;
      msg.color_range_end   = 0;
      memory_buf_t formatted;
      m_formatter->format(msg, formatted);
      if(should_do_colors_ && msg.color_range_end > msg.color_range_start)
      {
        // before color range
        print_range_impl(formatted, 0, msg.color_range_start);
        // in color range
        print_ccode_impl(m_colors[static_cast<size_t>(msg.level)]);
        print_range_impl(formatted, msg.color_range_start, msg.color_range_end);
        print_ccode_impl(reset);
        // after color range
        print_range_impl(formatted, msg.color_range_end, formatted.size());
      }
      else // no color
      {
        print_range_impl(formatted, 0, formatted.size());
      }
      fflush(m_target_file);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::flush()
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      fflush(m_target_file);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_pattern(const rex::DebugString& pattern)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_formatter = rsl::unique_ptr<rexlog::formatter>(new pattern_formatter(pattern));
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_formatter = rsl::move(sink_formatter);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE bool ansicolor_sink<ConsoleMutex>::should_color()
    {
      return should_do_colors_;
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_color_mode(color_mode mode)
    {
      switch(mode)
      {
        case color_mode::always: should_do_colors_ = true; return;
        case color_mode::automatic: should_do_colors_ = details::os::in_terminal(m_target_file) && details::os::is_color_terminal(); return;
        case color_mode::never: should_do_colors_ = false; return;
        default: should_do_colors_ = false;
      }
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::print_ccode_impl(const string_view_t& color_code)
    {
      fwrite(color_code.data(), sizeof(char), color_code.size(), m_target_file);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE void ansicolor_sink<ConsoleMutex>::print_range_impl(const memory_buf_t& formatted, size_t start, size_t end)
    {
      fwrite(formatted.data() + start, sizeof(char), end - start, m_target_file);
    }

    template <typename ConsoleMutex>
    REXLOG_INLINE rex::DebugString ansicolor_sink<ConsoleMutex>::to_string_impl(const string_view_t& sv)
    {
      return rex::DebugString(sv.data(), sv.size());
    }

    // ansicolor_stdout_sink
    template <typename ConsoleMutex>
    REXLOG_INLINE ansicolor_stdout_sink<ConsoleMutex>::ansicolor_stdout_sink(color_mode mode)
        : ansicolor_sink<ConsoleMutex>(stdout, mode)
    {
    }

    // ansicolor_stderr_sink
    template <typename ConsoleMutex>
    REXLOG_INLINE ansicolor_stderr_sink<ConsoleMutex>::ansicolor_stderr_sink(color_mode mode)
        : ansicolor_sink<ConsoleMutex>(stderr, mode)
    {
    }

  } // namespace sinks
} // namespace rexlog
