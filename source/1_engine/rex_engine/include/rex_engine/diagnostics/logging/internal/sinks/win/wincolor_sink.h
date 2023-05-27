

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/details/console_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_engine/diagnostics/logging/internal/details/windows_include.h"
#include "rex_std/memory.h"
#include "rex_std_extra/utility/enum_reflection.h"

namespace rexlog
{
  namespace sinks
  {
    /*
     * Windows color console sink. Uses WriteConsoleA to write to the console with
     * colors
     */
    template <typename ConsoleMutex>
    class WincolorSink : public AbstractSink
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
      void set_pattern(const rsl::small_stack_string& pattern) final;
      void set_formatter(PatternFormatter sinkFormatter) final;
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
      PatternFormatter m_formatter;
      rsl::array<rsl::uint16, rsl::enum_refl::enum_count<level::LevelEnum>()> m_colors;
    };

    template <typename ConsoleMutex>
    WincolorSink<ConsoleMutex>::WincolorSink(void* outHandle, ColorMode mode)
        : m_out_handle(outHandle)
        , m_mutex(&ConsoleMutex::mutex())
        , m_formatter()
    {
        set_color_mode_impl(mode);
        // set level colors
        m_colors[(int32)level::LevelEnum::Trace] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;                                               // white
        m_colors[(int32)level::LevelEnum::Debug] = FOREGROUND_GREEN | FOREGROUND_BLUE;                                                                // cyan
        m_colors[(int32)level::LevelEnum::Info] = FOREGROUND_GREEN;                                                                                   // green
        m_colors[(int32)level::LevelEnum::Warn] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;                                           // intense yellow
        m_colors[(int32)level::LevelEnum::Err] = FOREGROUND_RED | FOREGROUND_INTENSITY;                                                               // intense red
        m_colors[(int32)level::LevelEnum::Critical] = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;    // intense white on red background
        m_colors[(int32)level::LevelEnum::Off] = 0;
    }

    template <typename ConsoleMutex>
    WincolorSink<ConsoleMutex>::~WincolorSink()
    {
        this->flush();
    }

    // change the color for the given level
    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::set_color(level::LevelEnum level, rsl::uint16 color)
    {
        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        m_colors[static_cast<count_t>(level)] = color;
    }

    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::log(const details::LogMsg& msg)
    {
        if (m_out_handle == nullptr || m_out_handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        msg.color_range_start = 0;
        msg.color_range_end = 0;
        memory_buf_t formatted;
        m_formatter.format(msg, formatted);
        if (m_should_do_colors && msg.color_range_end > msg.color_range_start)
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
    void  WincolorSink<ConsoleMutex>::flush()
    {
        // windows console always flushed?
    }

    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::set_pattern(const rsl::small_stack_string& pattern)
    {
        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        m_formatter = PatternFormatter(pattern);
    }

    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::set_formatter(PatternFormatter sinkFormatter)
    {
        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        m_formatter = rsl::move(sinkFormatter);
    }

    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::set_color_mode(ColorMode mode)
    {
        const rsl::unique_lock<mutex_t> lock(*m_mutex);
        set_color_mode_impl(mode);
    }

    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::set_color_mode_impl(ColorMode mode)
    {
        if (mode == ColorMode::Automatic)
        {
            // should do colors only if out_handle_  points to actual console.
            DWORD console_mode = 0;
            const bool in_console = ::GetConsoleMode(static_cast<HANDLE>(m_out_handle), &console_mode) != 0;
            m_should_do_colors = in_console;
        }
        else
        {
            m_should_do_colors = mode == ColorMode::Always;
        }
    }

    // set foreground color and return the orig console attributes (for resetting later)
    template <typename ConsoleMutex>
    rsl::uint16  WincolorSink<ConsoleMutex>::set_foreground_color_impl(rsl::uint16 attribs)
    {
        CONSOLE_SCREEN_BUFFER_INFO orig_buffer_info;
        if (::GetConsoleScreenBufferInfo(static_cast<HANDLE>(m_out_handle), &orig_buffer_info) == 0)
        {
            // just return white if failed getting console info
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }

        // change only the foreground bits (lowest 4 bits)
        auto new_attribs = static_cast<WORD>(attribs) | (orig_buffer_info.wAttributes & 0xfff0); // NOLINT(hicpp-signed-bitwise)
        auto ignored = ::SetConsoleTextAttribute(static_cast<HANDLE>(m_out_handle), static_cast<WORD>(new_attribs));
        (void)(ignored);
        return static_cast<rsl::uint16>(orig_buffer_info.wAttributes); // return orig attribs
    }

    // print a range of formatted message to console
    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::print_range_impl(const memory_buf_t& formatted, size_t start, size_t end)
    {
        if (end > start)
        {
            auto size = static_cast<DWORD>(end - start);
            auto ignored = ::WriteConsoleA(static_cast<HANDLE>(m_out_handle), formatted.data() + start, size, nullptr, nullptr);
            (void)(ignored);
        }
    }

    template <typename ConsoleMutex>
    void  WincolorSink<ConsoleMutex>::write_to_file_impl(const memory_buf_t& formatted)
    {
        auto size = static_cast<DWORD>(formatted.size());
        DWORD bytes_written = 0;
        auto ignored = ::WriteFile(static_cast<HANDLE>(m_out_handle), formatted.data(), size, &bytes_written, nullptr);
        (void)(ignored);
    }

    template <typename ConsoleMutex>
    class WincolorStdoutSink : public WincolorSink<ConsoleMutex>
    {
    public:
      explicit WincolorStdoutSink(ColorMode mode = ColorMode::Automatic);
    };

    // wincolor_stdout_sink
    template <typename ConsoleMutex>
    WincolorStdoutSink<ConsoleMutex>::WincolorStdoutSink(ColorMode mode)
        : WincolorSink<ConsoleMutex>(::GetStdHandle(STD_OUTPUT_HANDLE), mode)
    {
    }

    template <typename ConsoleMutex>
    class WincolorStderrSink : public WincolorSink<ConsoleMutex>
    {
    public:
      explicit WincolorStderrSink(ColorMode mode = ColorMode::Automatic);
    };

    // wincolor_stderr_sink
    template <typename ConsoleMutex>
     WincolorStderrSink<ConsoleMutex>::WincolorStderrSink(ColorMode mode)
        : WincolorSink<ConsoleMutex>(::GetStdHandle(STD_ERROR_HANDLE), mode)
    {
    }

    using wincolor_stdout_sink_mt = WincolorStdoutSink<details::ConsoleMutex>;
    using wincolor_stdout_sink_st = WincolorStdoutSink<details::ConsoleNullMutex>;

    using wincolor_stderr_sink_mt = WincolorStderrSink<details::ConsoleMutex>;
    using wincolor_stderr_sink_st = WincolorStderrSink<details::ConsoleNullMutex>;
  } // namespace sinks
} // namespace rexlog