#pragma once

#include "rex_engine/debug_types.h"
#include "rex_engine/function_signature_define.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_std/atomic.h"
#include "rex_std/chrono.h"
#include "rex_std/format.h"
#include "rex_std/functional.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/exception/abort.h"
#include "rex_std/memory.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/type_traits.h"

#include <cstdio>
#include <version>

#undef REXLOG_HEADER_ONLY

#define REXLOG_API
#define REXLOG_INLINE

#define REXLOG_FMT_RUNTIME(format_string) format_string
#define REXLOG_FMT_STRING(format_string)  format_string

#define REXLOG_NOEXCEPT       noexcept
#define REXLOG_CONSTEXPR      constexpr
#define REXLOG_CONSTEXPR_FUNC constexpr

#if defined(__GNUC__) || defined(__clang__)
  #define REXLOG_DEPRECATED __attribute__impl((deprecated))
#elif defined(_MSC_VER)
  #define REXLOG_DEPRECATED __declspec(deprecated)
#else
  #define REXLOG_DEPRECATED
#endif

// disable thread local on msvc 2013
#ifndef REXLOG_NO_TLS
  #if(defined(_MSC_VER) && (_MSC_VER < 1900)) || defined(__cplusplus_winrt)
    #define REXLOG_NO_TLS 1
  #endif
#endif

#define REXLOG_FUNCTION REX_FUNC_SIG

#define REXLOG_TRY
#define REXLOG_THROW(ex)                                                                                                                                                                                                                                 \
  do                                                                                                                                                                                                                                                     \
  {                                                                                                                                                                                                                                                      \
    printf("rexlog fatal error: %s\n", ex.what());                                                                                                                                                                                                       \
    rsl::abort();                                                                                                                                                                                                                                        \
  } while(0)
#define REXLOG_CATCH_STD

namespace rexlog
{

  class formatter;

  namespace sinks
  {
    class sink;
  } // namespace sinks

  using filename_t = rex::DebugString;
#define REXLOG_FILENAME_T(s) s

  using log_clock       = rsl::chrono::system_clock;
  using sink_ptr        = rsl::shared_ptr<sinks::sink>;
  using sinks_init_list = rsl::initializer_list<sink_ptr>;
  using err_handler     = rsl::function<void(const rex::DebugString& errMsg)>;
  namespace fmt_lib     = rsl;

  using string_view_t = rsl::string_view;
  using memory_buf_t  = rex::DebugString;

  template <typename... Args>
#if __cpp_lib_format >= 202207L
  using format_string_t = rsl::format_string<Args...>;
#else
  using format_string_t = rsl::string_view;
#endif

  template <class T, class Char = char>
  struct IsConvertibleToBasicFormatString : rsl::integral_constant<bool, rsl::is_convertible<T, rsl::basic_string_view<Char>>::value>
  {
  };

#define REXLOG_BUF_TO_STRING(x) x

  template <class T>
  struct IsConvertibleToAnyFormatString : rsl::integral_constant<bool, IsConvertibleToBasicFormatString<T, char>::value || IsConvertibleToBasicFormatString<T, wchar_t>::value>
  {
  };

#if defined(REXLOG_NO_ATOMIC_LEVELS)
  using level_t = details::null_atomic_int;
#else
  using level_t         = rsl::atomic<int>;
#endif

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

  // Log level enum
  namespace level
  {
    enum LevelEnum : int
    {
      Trace    = REXLOG_LEVEL_TRACE,
      Debug    = REXLOG_LEVEL_DEBUG,
      Info     = REXLOG_LEVEL_INFO,
      Warn     = REXLOG_LEVEL_WARN,
      Err      = REXLOG_LEVEL_ERROR,
      Critical = REXLOG_LEVEL_CRITICAL,
      Off      = REXLOG_LEVEL_OFF
    };

#define REXLOG_LEVEL_NAME_TRACE    rexlog::string_view_t("trace", 5)
#define REXLOG_LEVEL_NAME_DEBUG    rexlog::string_view_t("debug", 5)
#define REXLOG_LEVEL_NAME_INFO     rexlog::string_view_t("info", 4)
#define REXLOG_LEVEL_NAME_WARNING  rexlog::string_view_t("warning", 7)
#define REXLOG_LEVEL_NAME_ERROR    rexlog::string_view_t("error", 5)
#define REXLOG_LEVEL_NAME_CRITICAL rexlog::string_view_t("critical", 8)
#define REXLOG_LEVEL_NAME_OFF      rexlog::string_view_t("off", 3)

#define REXLOG_LEVEL_NAMES                                                                                                                                                                                                                             \
    {                                                                                                                                                                                                                                                  \
      REXLOG_LEVEL_NAME_TRACE, REXLOG_LEVEL_NAME_DEBUG, REXLOG_LEVEL_NAME_INFO, REXLOG_LEVEL_NAME_WARNING, REXLOG_LEVEL_NAME_ERROR, REXLOG_LEVEL_NAME_CRITICAL, REXLOG_LEVEL_NAME_OFF                                                                  \
    }

#define REXLOG_SHORT_LEVEL_NAMES                                                                                                                                                                                                                       \
    {                                                                                                                                                                                                                                                  \
      "T", "D", "I", "W", "E", "C", "O"                                                                                                                                                                                                                \
    }

    REXLOG_API const string_view_t& to_string_view(rexlog::level::LevelEnum l) REXLOG_NOEXCEPT;
    REXLOG_API const char* to_short_c_str(rexlog::level::LevelEnum l) REXLOG_NOEXCEPT;
    REXLOG_API rexlog::level::LevelEnum from_str(const rex::DebugString& name) REXLOG_NOEXCEPT;

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

  //
  // Log exception
  //
  class REXLOG_API RexlogEx
  {
  public:
    explicit RexlogEx(rex::DebugString msg);
    RexlogEx(const rex::DebugString& msg, int lastErrno);
    const char* what() const REXLOG_NOEXCEPT;

  private:
    rex::DebugString m_msg;
  };

  REXLOG_API void throw_rexlog_ex(const rex::DebugString& msg, int lastErrno);
  REXLOG_API void throw_rexlog_ex(rex::DebugString msg);

  struct SourceLoc
  {
    REXLOG_CONSTEXPR SourceLoc() = default;
    REXLOG_CONSTEXPR SourceLoc(const char* filenameIn, int lineIn, const char* funcnameIn)
        : filename {filenameIn}
        , line {lineIn}
        , funcname {funcnameIn}
    {
    }

    REXLOG_CONSTEXPR bool empty() const REXLOG_NOEXCEPT
    {
      return line == 0;
    }
    const char* filename {nullptr};
    int line {0};
    const char* funcname {nullptr};
  };

  struct FileEventHandlers
  {
    FileEventHandlers()
        : before_open(nullptr)
        , after_open(nullptr)
        , before_close(nullptr)
        , after_close(nullptr)
    {
    }

    rsl::function<void(const filename_t& filename)> before_open;
    rsl::function<void(const filename_t& filename, FILE* fileStream)> after_open;
    rsl::function<void(const filename_t& filename, FILE* fileStream)> before_close;
    rsl::function<void(const filename_t& filename)> after_close;
  };

  namespace details
  {

    // to_string_view

    REXLOG_CONSTEXPR_FUNC rexlog::string_view_t to_string_view(const memory_buf_t& buf) REXLOG_NOEXCEPT
    {
      return rexlog::string_view_t {buf.data(), buf.size()};
    }

    REXLOG_CONSTEXPR_FUNC rexlog::string_view_t to_string_view(rexlog::string_view_t str) REXLOG_NOEXCEPT
    {
      return str;
    }

    template <typename T, typename... Args>
    REXLOG_CONSTEXPR_FUNC rsl::basic_string_view<T> to_string_view(rsl::basic_format_string<T, Args...> fmt) REXLOG_NOEXCEPT
    {
      return fmt.get();
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