#pragma once

#include "rex_engine/debug_types.h"
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
    using string_view_t     = rsl::string_view;
    using level_t           = rsl::atomic<int>;
    using memory_buf_t      = rex::DebugString;
    using log_clock         = rsl::chrono::system_clock;
    using sink_ptr          = rsl::shared_ptr<sinks::AbstractSink>;
    using sinks_init_list   = rsl::initializer_list<sink_ptr>;
    using err_handler       = rsl::function<void(const rex::DebugString& errMsg)>;
    
    template <typename... Args>
    using format_string_t   = rsl::format_string<Args...>;

    template <class T, class Char = char>
    struct IsConvertibleToBasicFormatString : rsl::integral_constant<bool, rsl::is_convertible<T, rsl::basic_string_view<Char>>::value>
    {
    };
    template <class T>
    struct IsConvertibleToAnyFormatString : rsl::integral_constant<bool, IsConvertibleToBasicFormatString<T, char>::value || IsConvertibleToBasicFormatString<T, wchar_t>::value>
    {
    };

#define REXLOG_LEVEL_TRACE    0
#define REXLOG_LEVEL_DEBUG    1
#define REXLOG_LEVEL_INFO     2
#define REXLOG_LEVEL_WARN     3
#define REXLOG_LEVEL_ERROR    4
#define REXLOG_LEVEL_CRITICAL 5
#define REXLOG_LEVEL_OFF      6

#if !defined(REXLOG_ACTIVE_LEVEL)
    #define REXLOG_ACTIVE_LEVEL REXLOG_LEVEL_INFO
#endif

    namespace level
    {
        #define REXLOG_LEVEL_NAME_TRACE     rexlog::string_view_t("trace", 5)
        #define REXLOG_LEVEL_NAME_DEBUG     rexlog::string_view_t("debug", 5)
        #define REXLOG_LEVEL_NAME_INFO      rexlog::string_view_t("info", 4)
        #define REXLOG_LEVEL_NAME_WARNING   rexlog::string_view_t("warning", 7)
        #define REXLOG_LEVEL_NAME_ERROR     rexlog::string_view_t("error", 5)
        #define REXLOG_LEVEL_NAME_CRITICAL  rexlog::string_view_t("critical", 8)
        #define REXLOG_LEVEL_NAME_OFF       rexlog::string_view_t("off", 3)

        #define REXLOG_LEVEL_SNAME_TRACE    rexlog::string_view_t("T", 1)
        #define REXLOG_LEVEL_SNAME_DEBUG    rexlog::string_view_t("D", 1)
        #define REXLOG_LEVEL_SNAME_INFO     rexlog::string_view_t("I", 1)
        #define REXLOG_LEVEL_SNAME_WARNING  rexlog::string_view_t("W", 1)
        #define REXLOG_LEVEL_SNAME_ERROR    rexlog::string_view_t("E", 1)
        #define REXLOG_LEVEL_SNAME_CRITICAL rexlog::string_view_t("C", 1)
        #define REXLOG_LEVEL_SNAME_OFF      rexlog::string_view_t("O", 1)

        enum class LevelEnum : int32
        {
            Trace       = REXLOG_LEVEL_TRACE,
            Debug       = REXLOG_LEVEL_DEBUG,
            Info        = REXLOG_LEVEL_INFO,
            Warn        = REXLOG_LEVEL_WARN,
            Err         = REXLOG_LEVEL_ERROR,
            Critical    = REXLOG_LEVEL_CRITICAL,
            Off         = REXLOG_LEVEL_OFF
        };

        string_view_t               to_string_view(rexlog::level::LevelEnum l) noexcept;
        string_view_t               to_short_c_str(rexlog::level::LevelEnum l) noexcept;

        rexlog::level::LevelEnum    from_str(const rex::DebugString& name) noexcept;

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

        constexpr rexlog::string_view_t to_string_view(const memory_buf_t& buf) noexcept
        {
            return rexlog::string_view_t{ buf.data(), buf.size() };
        }

        constexpr rexlog::string_view_t to_string_view(rexlog::string_view_t str) noexcept
        {
            return str;
        }

        template <typename T, typename... Args>
        constexpr rsl::basic_string_view<T> to_string_view(rsl::basic_format_string<T, Args...> fmt) noexcept
        {
            return rsl::basic_string_view<T>(fmt);
        }

        using rsl::enable_if_t;
        using rsl::make_unique;

        // to avoid useless casts (see https://github.com/nlohmann/json/issues/2893#issuecomment-889152324)
        template <typename T, typename U, enable_if_t<!rsl::is_same<T, U>::value, int> = 0>
        constexpr T conditional_static_cast(U value)
        {
            return static_cast<T>(value);
        }

        template <typename T, typename U, enable_if_t<rsl::is_same<T, U>::value, int> = 0>
        constexpr T conditional_static_cast(U value)
        {
            return value;
        }

    } // namespace details
} // namespace rexlog