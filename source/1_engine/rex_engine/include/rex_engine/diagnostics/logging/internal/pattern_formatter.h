

#pragma once

#include "rex_engine/engine/constants.h"
#include "rex_engine/engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/formatting/flag_formatter.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_std/bonus/string.h"
#include "rex_std/chrono.h"
#include "rex_std/memory.h"

#include <corecrt_wtime.h>

namespace rexlog
{

  namespace details
  {
    struct PaddingInfo;
    class LogMsg;
  } // namespace details

  class PatternFormatter
  {
  public:
    explicit PatternFormatter(rsl::string_view pattern, PatternTimeType timeType = PatternTimeType::Local, rsl::string_view eol = rex::g_default_eol);
    explicit PatternFormatter(PatternTimeType timeType = PatternTimeType::Local, rsl::string_view eol = rex::g_default_eol);

    void format(const details::LogMsg& msg, rsl::big_stack_string& dest);
    void set_pattern(rsl::string_view pattern);
    void need_localtime(bool need = true);

    PatternFormatter clone() const;

  private:
    using Formatters = rex::DebugVector<rsl::unique_ptr<details::FlagFormatter>>;

    template <typename Padder>
    void handle_flag_impl(char flag, details::PaddingInfo padding);
    void compile_pattern_impl(rsl::string_view pattern);

    rsl::small_stack_string m_pattern;
    rsl::tiny_stack_string m_eol;

    PatternTimeType m_pattern_time_type;
    bool m_need_localtime;
    tm m_cached_tm;
    rsl::chrono::seconds m_last_log_secs;
    Formatters m_formatters;

    static constexpr rsl::string_view s_default_pattern = "%^[%T][%=8l] %n: %v%$";
  };
} // namespace rexlog