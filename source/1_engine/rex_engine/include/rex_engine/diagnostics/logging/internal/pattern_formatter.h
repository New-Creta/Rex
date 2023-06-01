

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/details/formatting/flag_formatter.h"
#include "rex_engine/types.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/string/stack_string.h"

namespace rexlog
{
    struct LogMsg;

    namespace details
    {
        struct PaddingInfo;
    }

    class PatternFormatter
    {
    public:
        explicit PatternFormatter(rsl::string_view pattern, PatternTimeType timeType = PatternTimeType::Local, const rsl::tiny_stack_string& eol = rsl::tiny_stack_string(rexlog::details::os::default_eol));
        explicit PatternFormatter(PatternTimeType timeType = PatternTimeType::Local, const rsl::tiny_stack_string& eol = rsl::tiny_stack_string(rexlog::details::os::default_eol));

        void format(const details::LogMsg& msg, memory_buf_t& dest);
        void set_pattern(const rsl::small_stack_string& pattern);
        void need_localtime(bool need = true);

        PatternFormatter clone() const;

    private:
        using Formatters = rex::DebugVector<rsl::unique_ptr<details::FlagFormatter>>;

        template <typename Padder>
        void handle_flag_impl(char flag, details::PaddingInfo padding);
        void compile_pattern_impl(const rsl::small_stack_string& pattern);

        rsl::small_stack_string m_pattern;
        rsl::tiny_stack_string  m_eol;

        PatternTimeType         m_pattern_time_type;
        bool                    m_need_localtime;
        tm                      m_cached_tm;
        rsl::chrono::seconds    m_last_log_secs;
        Formatters              m_formatters;
    };
} // namespace rexlog