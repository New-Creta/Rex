

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/console_globals.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_std/memory.h"

#include <array>
#include <mutex>
#include <string>

namespace rexlog
{
  namespace sinks
  {

    /**
     * This sink prefixes the output with an ANSI escape sequence color code
     * depending on the severity
     * of the message.
     * If no color terminal detected, omit the escape codes.
     */

    template <typename ConsoleMutex>
    class ansicolor_sink : public sink
    {
    public:
      using mutex_t = typename ConsoleMutex::mutex_t;
      ansicolor_sink(FILE* target_file, ColorMode mode);
      ~ansicolor_sink() override = default;

      ansicolor_sink(const ansicolor_sink& other) = delete;
      ansicolor_sink(ansicolor_sink&& other)      = delete;

      ansicolor_sink& operator=(const ansicolor_sink& other) = delete;
      ansicolor_sink& operator=(ansicolor_sink&& other)      = delete;

      void set_color(level::level_enum color_level, string_view_t color);
      void set_color_mode(ColorMode mode);
      bool should_color();

      void log(const details::LogMsg& msg) override;
      void flush() override;
      void set_pattern(const rex::DebugString& pattern) final;
      void set_formatter(rsl::unique_ptr<rexlog::formatter> sink_formatter) override;

      // Formatting codes
      const string_view_t reset      = "\033[m";
      const string_view_t bold       = "\033[1m";
      const string_view_t dark       = "\033[2m";
      const string_view_t underline  = "\033[4m";
      const string_view_t blink      = "\033[5m";
      const string_view_t reverse    = "\033[7m";
      const string_view_t concealed  = "\033[8m";
      const string_view_t clear_line = "\033[K";

      // Foreground colors
      const string_view_t black   = "\033[30m";
      const string_view_t red     = "\033[31m";
      const string_view_t green   = "\033[32m";
      const string_view_t yellow  = "\033[33m";
      const string_view_t blue    = "\033[34m";
      const string_view_t magenta = "\033[35m";
      const string_view_t cyan    = "\033[36m";
      const string_view_t white   = "\033[37m";

      /// Background colors
      const string_view_t on_black   = "\033[40m";
      const string_view_t on_red     = "\033[41m";
      const string_view_t on_green   = "\033[42m";
      const string_view_t on_yellow  = "\033[43m";
      const string_view_t on_blue    = "\033[44m";
      const string_view_t on_magenta = "\033[45m";
      const string_view_t on_cyan    = "\033[46m";
      const string_view_t on_white   = "\033[47m";

      /// Bold colors
      const string_view_t yellow_bold = "\033[33m\033[1m";
      const string_view_t red_bold    = "\033[31m\033[1m";
      const string_view_t bold_on_red = "\033[1m\033[41m";

    private:
      FILE* m_target_file;
      mutex_t& m_mutex;
      bool should_do_colors_;
      rsl::unique_ptr<rexlog::formatter> m_formatter;
      rsl::array<rex::DebugString, level::n_levels> m_colors;
      void print_ccode_impl(const string_view_t& color_code);
      void print_range_impl(const memory_buf_t& formatted, size_t start, size_t end);
      static rex::DebugString to_string_impl(const string_view_t& sv);
    };

    template <typename ConsoleMutex>
    class ansicolor_stdout_sink : public ansicolor_sink<ConsoleMutex>
    {
    public:
      explicit ansicolor_stdout_sink(ColorMode mode = ColorMode::automatic);
    };

    template <typename ConsoleMutex>
    class ansicolor_stderr_sink : public ansicolor_sink<ConsoleMutex>
    {
    public:
      explicit ansicolor_stderr_sink(ColorMode mode = ColorMode::automatic);
    };

    using ansicolor_stdout_sink_mt = ansicolor_stdout_sink<details::ConsoleMutex>;
    using ansicolor_stdout_sink_st = ansicolor_stdout_sink<details::ConsoleNullMutex>;

    using ansicolor_stderr_sink_mt = ansicolor_stderr_sink<details::ConsoleMutex>;
    using ansicolor_stderr_sink_st = ansicolor_stderr_sink<details::ConsoleNullMutex>;

  } // namespace sinks
} // namespace rexlog