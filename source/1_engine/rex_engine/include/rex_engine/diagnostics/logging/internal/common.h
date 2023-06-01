#pragma once

#include "rex_engine/debug_types.h"
#include "rex_engine/types.h"
#include "rex_engine/function_signature_define.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_std/bonus/atomic/atomic.h"
#include "rex_std/bonus/compiler.h"
#include "rex_std/bonus/memory.h"
#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/exception/abort.h"
#include "rex_std/memory.h"
#include "rex_std/string.h"

#include <cstdio>

// IWYU pragma: no_include <built-in>

#define REXLOG_FUNCTION REX_FUNC_SIG

namespace rexlog
{
    namespace sinks
    {
        class AbstractSink;
    } // namespace sinks

    using filename_t        = rex::DebugString;
    using memory_buf_t      = rex::DebugString;
    using level_t           = rsl::atomic<int>;
    using log_clock         = rsl::chrono::system_clock;
    using sink_ptr          = rsl::shared_ptr<sinks::AbstractSink>;
    using sinks_init_list   = rsl::initializer_list<sink_ptr>;
    using err_handler       = rsl::function<void(const rex::DebugString& errMsg)>;
    
    template <typename... Args>
    using format_string_t   = rsl::format_string<Args...>;

    template <class T, class Char = char>
    struct IsConvertibleToBasicStringView : rsl::integral_constant<bool, rsl::is_convertible<T, rsl::basic_string_view<Char>>::value>
    {
    };
    template <class T>
    struct IsConvertibleToAnyFormatString : rsl::integral_constant<bool, IsConvertibleToBasicStringView<T, char>::value || IsConvertibleToBasicStringView<T, wchar_t>::value>
    {
    };

    inline constexpr s8 g_level_trace = 0;
    inline constexpr s8 g_level_debug = 1;
    inline constexpr s8 g_level_info = 2;
    inline constexpr s8 g_level_warning = 3;
    inline constexpr s8 g_level_error = 4;
    inline constexpr s8 g_level_critical = 5;
    inline constexpr s8 g_level_off = 6;

#if !defined(REXLOG_ACTIVE_LEVEL)
    #define REXLOG_ACTIVE_LEVEL g_level_info
#endif

    namespace level
    {
        inline rsl::string_view g_level_name_trace("trace");
        inline rsl::string_view g_level_name_debug("debug");
        inline rsl::string_view g_level_name_info("info");
        inline rsl::string_view g_level_name_warning("warning");
        inline rsl::string_view g_level_name_error("error");
        inline rsl::string_view g_level_name_critical("critical");
        inline rsl::string_view g_level_name_off("off");

        inline rsl::string_view g_level_sname_trace("T");
        inline rsl::string_view g_level_sname_debug("D");
        inline rsl::string_view g_level_sname_info("I");
        inline rsl::string_view g_level_sname_warning("W");
        inline rsl::string_view g_level_sname_error("E");
        inline rsl::string_view g_level_sname_critical("C");
        inline rsl::string_view g_level_sname_off("O");

        enum class LevelEnum : int32
        {
            Trace       = g_level_trace,
            Debug       = g_level_debug,
            Info        = g_level_info,
            Warn        = g_level_warning,
            Err         = g_level_error,
            Critical    = g_level_critical,
            Off         = g_level_off
        };

        rsl::string_view            to_string_view(rexlog::level::LevelEnum l) noexcept;
        rsl::string_view            to_short_c_str(rexlog::level::LevelEnum l) noexcept;

        rexlog::level::LevelEnum    from_str(rsl::string_view name) noexcept;

    } // namespace level

    //
    // Color mode used by sinks with color support.
    //
    enum class ColorMode
    {
        Always,
        Automatic,
        Never
    };

    //
    // Pattern time - specific time getting to use for pattern_formatter.
    // local time by default
    //
    enum class PatternTimeType
    {
        Local, // log localtime
        Utc    // log utc
    };

    namespace details
    {
        // to_string_view

        constexpr rsl::string_view to_string_view(const memory_buf_t& buf) noexcept
        {
            return rsl::string_view{ buf.data(), buf.size() };
        }

        constexpr rsl::string_view to_string_view(rsl::string_view str) noexcept
        {
            return str;
        }

        template <typename T, typename... Args>
        constexpr rsl::basic_string_view<T> to_string_view(rsl::basic_format_string<T, Args...> fmt) noexcept
        {
            return rsl::basic_string_view<T>(fmt);
        }

        // to avoid useless casts (see https://github.com/nlohmann/json/issues/2893#issuecomment-889152324)
        template <typename T, typename U, rsl::enable_if_t<!rsl::is_same<T, U>::value, int> = 0>
        constexpr T conditional_static_cast(U value)
        {
            return static_cast<T>(value);
        }

        template <typename T, typename U, rsl::enable_if_t<rsl::is_same<T, U>::value, int> = 0>
        constexpr T conditional_static_cast(U value)
        {
            return value;
        }

    } // namespace details
} // namespace rexlog