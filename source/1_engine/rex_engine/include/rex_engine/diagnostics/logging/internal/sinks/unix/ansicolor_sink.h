#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/console_mutex.h"
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
    class ANSIColorSink : public AbstractSink
    {
    public:
      using mutex_t = typename ConsoleMutex::mutex_t;
      ANSIColorSink(FILE* target_file, ColorMode mode);
      ~ANSIColorSink() override = default;

      ANSIColorSink(const ANSIColorSink& other) = delete;
      ANSIColorSink(ANSIColorSink&& other)      = delete;

      ANSIColorSink& operator=(const ANSIColorSink& other) = delete;
      ANSIColorSink& operator=(ANSIColorSink&& other)      = delete;

      void set_color(level::LevelEnum color_level, rsl::string_view color);
      void set_color_mode(ColorMode mode);
      bool should_color();

      void log(const details::LogMsg& msg) override;
      void flush() override;
      void set_pattern(rsl::string_view pattern) final;
      void set_formatter(PatternFormatter sink_formatter) override;

      // Formatting codes
      const rsl::string_view reset      = "\033[m";
      const rsl::string_view bold       = "\033[1m";
      const rsl::string_view dark       = "\033[2m";
      const rsl::string_view underline  = "\033[4m";
      const rsl::string_view blink      = "\033[5m";
      const rsl::string_view reverse    = "\033[7m";
      const rsl::string_view concealed  = "\033[8m";
      const rsl::string_view clear_line = "\033[K";

      // Foreground colors
      const rsl::string_view black   = "\033[30m";
      const rsl::string_view red     = "\033[31m";
      const rsl::string_view green   = "\033[32m";
      const rsl::string_view yellow  = "\033[33m";
      const rsl::string_view blue    = "\033[34m";
      const rsl::string_view magenta = "\033[35m";
      const rsl::string_view cyan    = "\033[36m";
      const rsl::string_view white   = "\033[37m";

      /// Background colors
      const rsl::string_view on_black   = "\033[40m";
      const rsl::string_view on_red     = "\033[41m";
      const rsl::string_view on_green   = "\033[42m";
      const rsl::string_view on_yellow  = "\033[43m";
      const rsl::string_view on_blue    = "\033[44m";
      const rsl::string_view on_magenta = "\033[45m";
      const rsl::string_view on_cyan    = "\033[46m";
      const rsl::string_view on_white   = "\033[47m";

      /// Bold colors
      const rsl::string_view yellow_bold = "\033[33m\033[1m";
      const rsl::string_view red_bold    = "\033[31m\033[1m";
      const rsl::string_view bold_on_red = "\033[1m\033[41m";

    private:
      void print_ccode_impl(rsl::string_view color_code);
      void print_range_impl(rsl::string_view formatted, s32 start, s32 end);

    private:
      FILE* m_target_file;
      mutex_t& m_mutex;
      bool should_do_colors_;
      PatternFormatter m_formatter;
      rsl::array<rsl::string_view, rsl::enum_refl::enum_count<level::LevelEnum>()> m_colors;
    };

    template <typename ConsoleMutex>
    ANSIColorSink<ConsoleMutex>::ANSIColorSink(FILE* target_file, ColorMode mode)
        : m_target_file(target_file)
        , m_mutex(ConsoleMutex::mutex())
        , m_formatter(rsl::make_unique<rexlog::pattern_formatter>())

    {
      set_color_mode(mode);

      m_colors[level::trace]    = white;
      m_colors[level::debug]    = cyan;
      m_colors[level::info]     = green;
      m_colors[level::warn]     = yellow_bold;
      m_colors[level::err]      = red_bold;
      m_colors[level::critical] = bold_on_red;
      m_colors[level::off]      = reset;
    }

    template <typename ConsoleMutex>
    void ANSIColorSink<ConsoleMutex>::set_color(level::LevelEnum color_level, rsl::string_view color)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_colors[static_cast<s32>(color_level)] = color;
    }

    template <typename ConsoleMutex>
    void ANSIColorSink<ConsoleMutex>::log(const details::LogMsg& msg)
    {
      // Wrap the originally formatted message in color codes.
      // If color is not supported in the terminal, log as is instead.
      rsl::lock_guard<mutex_t> lock(m_mutex);
      msg.color_range_start = 0;
      msg.color_range_end   = 0;
      rsl::big_stack_string formatted;
      m_formatter->format(msg, formatted);
      if(should_do_colors_ && msg.color_range_end > msg.color_range_start)
      {
        // before color range
        print_range_impl(formatted, 0, msg.color_range_start);
        // in color range
        print_ccode_impl(m_colors[static_cast<s32>(msg.level)]);
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
    void ANSIColorSink<ConsoleMutex>::flush()
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      fflush(m_target_file);
    }

    template <typename ConsoleMutex>
    void ANSIColorSink<ConsoleMutex>::set_pattern(rsl::string_view pattern)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_formatter = PatternFormatter(pattern);
    }

    template <typename ConsoleMutex>
    void ANSIColorSink<ConsoleMutex>::set_formatter(PatternFormatter sink_formatter)
    {
      rsl::lock_guard<mutex_t> lock(m_mutex);
      m_formatter = rsl::move(sink_formatter);
    }

    template <typename ConsoleMutex>
    bool ANSIColorSink<ConsoleMutex>::should_color()
    {
      return should_do_colors_;
    }

    template <typename ConsoleMutex>
    void ANSIColorSink<ConsoleMutex>::set_color_mode(ColorMode mode)
    {
      switch(mode)
      {
        case ColorMode::Always: should_do_colors_ = true; return;
        case ColorMode::Automatic: should_do_colors_ = details::os::in_terminal(m_target_file) && details::os::is_color_terminal(); return;
        case ColorMode::Never: should_do_colors_ = false; return;
        default: should_do_colors_ = false;
      }
    }

    template <typename ConsoleMutex>
    void ANSIColorSink<ConsoleMutex>::print_ccode_impl(rsl::string_view color_code)
    {
      fwrite(color_code.data(), sizeof(char), color_code.size(), m_target_file);
    }

    template <typename ConsoleMutex>
    void ANSIColorSink<ConsoleMutex>::print_range_impl(rsl::string_view formatted, s32 start, s32 end)
    {
      fwrite(formatted.data() + start, sizeof(char), end - start, m_target_file);
    }

    template <typename ConsoleMutex>
    class ANSIColorStdoutSink : public ANSIColorSink<ConsoleMutex>
    {
    public:
      explicit ANSIColorStdoutSink(ColorMode mode = ColorMode::Automatic);
    };

    // ANSIColorStdoutSink
    template <typename ConsoleMutex>
    ANSIColorStdoutSink<ConsoleMutex>::ANSIColorStdoutSink(ColorMode mode)
        : ANSIColorSink<ConsoleMutex>(stdout, mode)
    {
    }

    template <typename ConsoleMutex>
    class ANSIColorStderrSink : public ANSIColorSink<ConsoleMutex>
    {
    public:
      explicit ANSIColorStderrSink(ColorMode mode = ColorMode::Automatic);
    };

    // ANSIColorStderrSink
    template <typename ConsoleMutex>
    ANSIColorStderrSink<ConsoleMutex>::ANSIColorStderrSink(ColorMode mode)
        : ANSIColorSink<ConsoleMutex>(stderr, mode)
    {
    }

    using ANSIColorStdoutSinkMt = ANSIColorStdoutSink<details::ConsoleMutex>;
    using ANSIColorStdoutSinkSt = ANSIColorStdoutSink<details::ConsoleNullMutex>;

    using ANSIColorStderrSinkMt = ANSIColorStderrSink<details::ConsoleMutex>;
    using ANSIColorStderrSinkSt = ANSIColorStderrSink<details::ConsoleNullMutex>;

  } // namespace sinks
} // namespace rexlog