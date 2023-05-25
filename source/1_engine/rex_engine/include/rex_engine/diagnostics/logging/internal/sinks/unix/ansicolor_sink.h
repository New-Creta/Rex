

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/console_globals.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_std/memory.h"
#include "rex_std_extra/utility/enum_reflection.h"


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
    class ansicolor_sink : public AbstractSink
    {
    public:
      using mutex_t = typename ConsoleMutex::mutex_t;
      ansicolor_sink(FILE* target_file, ColorMode mode);
      ~ansicolor_sink() override = default;

      ansicolor_sink(const ansicolor_sink& other) = delete;
      ansicolor_sink(ansicolor_sink&& other)      = delete;

      ansicolor_sink& operator=(const ansicolor_sink& other) = delete;
      ansicolor_sink& operator=(ansicolor_sink&& other)      = delete;

      void set_color(level::LevelEnum color_level, string_view_t color);
      void set_color_mode(ColorMode mode);
      bool should_color();

      void log(const details::LogMsg& msg) override;
      void flush() override;
      void set_pattern(const rsl::small_stack_string& pattern) final;
      void set_formatter(PatternFormatter sink_formatter) override;

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
      PatternFormatter m_formatter;
      rsl::array<rex::DebugString, rsl::enum_refl::enum_count<level::LevelEnum>()> m_colors;
      void print_ccode_impl(const string_view_t& color_code);
      void print_range_impl(const memory_buf_t& formatted, size_t start, size_t end);
      static rex::DebugString to_string_impl(const string_view_t& sv);
    };

    template <typename ConsoleMutex>
    ansicolor_sink<ConsoleMutex>::ansicolor_sink(FILE* target_file, ColorMode mode)
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
    void ansicolor_sink<ConsoleMutex>::set_color(level::LevelEnum color_level, string_view_t color)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_colors[static_cast<size_t>(color_level)] = to_string_impl(color);
    }

    template <typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::log(const details::LogMsg& msg)
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
    void ansicolor_sink<ConsoleMutex>::flush()
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      fflush(m_target_file);
    }

    template <typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::set_pattern(const rsl::small_stack_string& pattern)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_formatter = PatternFormatter(pattern);
    }

    template <typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::set_formatter(PatternFormatter sink_formatter)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_formatter = rsl::move(sink_formatter);
    }

    template <typename ConsoleMutex>
    bool ansicolor_sink<ConsoleMutex>::should_color()
    {
      return should_do_colors_;
    }

    template <typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::set_color_mode(ColorMode mode)
    {
      switch(mode)
      {
        case ColorMode::Always: should_do_colors_ = true; return;
        case ColorMode::automatic: should_do_colors_ = details::os::in_terminal(m_target_file) && details::os::is_color_terminal(); return;
        case ColorMode::never: should_do_colors_ = false; return;
        default: should_do_colors_ = false;
      }
    }

    template <typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::print_ccode_impl(const string_view_t& color_code)
    {
      fwrite(color_code.data(), sizeof(char), color_code.size(), m_target_file);
    }

    template <typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::print_range_impl(const memory_buf_t& formatted, size_t start, size_t end)
    {
      fwrite(formatted.data() + start, sizeof(char), end - start, m_target_file);
    }

    template <typename ConsoleMutex>
    rex::DebugString ansicolor_sink<ConsoleMutex>::to_string_impl(const string_view_t& sv)
    {
      return rex::DebugString(sv.data(), sv.size());
    }

    template <typename ConsoleMutex>
    class ansicolor_stdout_sink : public ansicolor_sink<ConsoleMutex>
    {
    public:
      explicit ansicolor_stdout_sink(ColorMode mode = ColorMode::Automatic);
    };

    // ansicolor_stdout_sink
    template <typename ConsoleMutex>
    ansicolor_stdout_sink<ConsoleMutex>::ansicolor_stdout_sink(ColorMode mode)
        : ansicolor_sink<ConsoleMutex>(stdout, mode)
    {
    }

    template <typename ConsoleMutex>
    class ansicolor_stderr_sink : public ansicolor_sink<ConsoleMutex>
    {
    public:
      explicit ansicolor_stderr_sink(ColorMode mode = ColorMode::Automatic);
    };

    // ansicolor_stderr_sink
    template <typename ConsoleMutex>
    ansicolor_stderr_sink<ConsoleMutex>::ansicolor_stderr_sink(ColorMode mode)
        : ansicolor_sink<ConsoleMutex>(stderr, mode)
    {
    }

    using ansicolor_stdout_sink_mt = ansicolor_stdout_sink<details::ConsoleMutex>;
    using ansicolor_stdout_sink_st = ansicolor_stdout_sink<details::ConsoleNullMutex>;

    using ansicolor_stderr_sink_mt = ansicolor_stderr_sink<details::ConsoleMutex>;
    using ansicolor_stderr_sink_st = ansicolor_stderr_sink<details::ConsoleNullMutex>;

  } // namespace sinks
} // namespace rexlog