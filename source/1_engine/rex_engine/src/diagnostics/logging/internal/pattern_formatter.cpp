#include "rex_engine/diagnostics/logging/internal/details/fmt_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/diagnostics/logging/internal/details/formatting/padding_info.h"
#include "rex_engine/diagnostics/logging/internal/details/formatting/padder.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

// NOLINTBEGIN(readability-identifier-naming)
// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
    namespace details
    {
        //-------------------------------------------------------------------------
        tm get_time_impl(PatternTimeType timeType, const details::LogMsg& msg)
        {
            if (timeType == PatternTimeType::Local)
            {
                return details::os::localtime(log_clock::to_time_t(msg.time));
            }

            return details::os::gmtime(log_clock::to_time_t(msg.time));
        }

        //-------------------------------------------------------------------------
        // Extract given pad spec (e.g. %8X, %=8X, %-8!X, %8!X, %=8!X, %-8!X, %+8!X)
        // Advance the given it pass the end of the padding spec found (if any)
        // Return padding.
        details::PaddingInfo handle_padspec_impl(rex::DebugString::const_iterator& it, rex::DebugString::const_iterator end)
        {
            using details::PaddingInfo;
            using details::ScopedPadder;

            const size_t max_width = 64;
            if (it == end)
            {
                return PaddingInfo{};
            }

            PaddingInfo::PadSide side = PaddingInfo::PadSide::Center;
            switch (*it)
            {
            case '-':
                side = PaddingInfo::PadSide::Right;
                ++it;
                break;
            case '=':
                side = PaddingInfo::PadSide::Center;
                ++it;
                break;
            default: side = details::PaddingInfo::PadSide::Left; break;
            }

            if (it == end || !rsl::is_digit(static_cast<unsigned char>(*it)))
            {
                return PaddingInfo{}; // no padding if no digit found here
            }

            auto width = static_cast<size_t>(*it) - '0';
            for (++it; it != end && rsl::is_digit(static_cast<unsigned char>(*it)); ++it)
            {
                auto digit = static_cast<size_t>(*it) - '0';
                width = width * 10 + digit;
            }

            // search for the optional truncate marker '!'
            bool truncate = false;
            if (it != end && *it == '!')
            {
                truncate = true;
                ++it;
            }
            else
            {
                truncate = false;
            }
            return details::PaddingInfo{ rsl::min<size_t>(width, max_width), side, truncate };
        }

    } // namespace details

    //-------------------------------------------------------------------------
    PatternFormatter::PatternFormatter(const rsl::small_stack_string& pattern, PatternTimeType timeType, const rsl::tiny_stack_string& eol)
        : m_pattern(rsl::move(pattern))
        , m_eol(rsl::move(eol))
        , m_pattern_time_type(timeType)
        , m_need_localtime(false)
        , m_cached_tm({})
        , m_last_log_secs(0)
    {
        rsl::memset(&m_cached_tm, 0, sizeof(m_cached_tm));
        compile_pattern_impl(m_pattern);
    }

    //-------------------------------------------------------------------------
    // use by default full formatter for if pattern is not given
    PatternFormatter::PatternFormatter(PatternTimeType timeType, const rsl::tiny_stack_string& eol)
        : m_pattern("%+")
        , m_eol(rsl::move(eol))
        , m_pattern_time_type(timeType)
        , m_need_localtime(true)
        , m_cached_tm({})
        , m_last_log_secs(0)
    {
        rsl::memset(&m_cached_tm, 0, sizeof(m_cached_tm));
        m_formatters.push_back(details::make_unique<details::full_formatter>(details::PaddingInfo{}));
    }

    //-------------------------------------------------------------------------
    void PatternFormatter::format(const details::LogMsg& msg, memory_buf_t& dest)
    {
        if (m_need_localtime)
        {
            const auto secs = rsl::chrono::duration_cast<rsl::chrono::seconds>(msg.time.time_since_epoch());
            if (secs != m_last_log_secs)
            {
                m_cached_tm = details::get_time_impl(m_pattern_time_type, msg);
                m_last_log_secs = secs;
            }
        }

        for (auto& f : m_formatters)
        {
            f->format(msg, m_cached_tm, dest);
        }

        details::fmt_helper::append_string_view(m_eol, dest);
    }

    //-------------------------------------------------------------------------
    void PatternFormatter::set_pattern(const rsl::small_stack_string& pattern)
    {
        m_pattern = rsl::move(pattern);
        m_need_localtime = false;
        compile_pattern_impl(m_pattern);
    }

    //-------------------------------------------------------------------------
    void PatternFormatter::need_localtime(bool need)
    {
        m_need_localtime = need;
    }

    //-------------------------------------------------------------------------
    PatternFormatter PatternFormatter::clone() const
    {
        auto cloned = PatternFormatter(m_pattern, m_pattern_time_type, m_eol);

        cloned.need_localtime(m_need_localtime);

        return cloned;
    }

    //-------------------------------------------------------------------------
    template <typename Padder>
    void PatternFormatter::handle_flag_impl(char flag, details::PaddingInfo padding)
    {
        // process built-in flags
        switch (flag)
        {
        case('+'): // default formatter
            m_formatters.push_back(details::make_unique<details::full_formatter>(padding));
            m_need_localtime = true;
            break;

        case 'n': // logger name
            m_formatters.push_back(details::make_unique<details::name_formatter<Padder>>(padding));
            break;

        case 'l': // level
            m_formatters.push_back(details::make_unique<details::level_formatter<Padder>>(padding));
            break;

        case 'L': // short level
            m_formatters.push_back(details::make_unique<details::short_level_formatter<Padder>>(padding));
            break;

        case('t'): // thread id
            m_formatters.push_back(details::make_unique<details::t_formatter<Padder>>(padding));
            break;

        case('v'): // the message text
            m_formatters.push_back(details::make_unique<details::v_formatter<Padder>>(padding));
            break;

        case('a'): // weekday
            m_formatters.push_back(details::make_unique<details::a_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('A'): // short weekday
            m_formatters.push_back(details::make_unique<details::A_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('b'):
        case('h'): // month
            m_formatters.push_back(details::make_unique<details::b_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('B'): // short month
            m_formatters.push_back(details::make_unique<details::B_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('c'): // datetime
            m_formatters.push_back(details::make_unique<details::c_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('C'): // year 2 digits
            m_formatters.push_back(details::make_unique<details::C_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('Y'): // year 4 digits
            m_formatters.push_back(details::make_unique<details::Y_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('D'):
        case('x'): // datetime MM/DD/YY
            m_formatters.push_back(details::make_unique<details::D_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('m'): // month 1-12
            m_formatters.push_back(details::make_unique<details::m_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('d'): // day of month 1-31
            m_formatters.push_back(details::make_unique<details::d_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('H'): // hours 24
            m_formatters.push_back(details::make_unique<details::H_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('I'): // hours 12
            m_formatters.push_back(details::make_unique<details::I_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('M'): // minutes
            m_formatters.push_back(details::make_unique<details::M_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('S'): // seconds
            m_formatters.push_back(details::make_unique<details::S_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('e'): // milliseconds
            m_formatters.push_back(details::make_unique<details::e_formatter<Padder>>(padding));
            break;

        case('f'): // microseconds
            m_formatters.push_back(details::make_unique<details::f_formatter<Padder>>(padding));
            break;

        case('F'): // nanoseconds
            m_formatters.push_back(details::make_unique<details::F_formatter<Padder>>(padding));
            break;

        case('E'): // seconds since epoch
            m_formatters.push_back(details::make_unique<details::E_formatter<Padder>>(padding));
            break;

        case('p'): // am/pm
            m_formatters.push_back(details::make_unique<details::p_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('r'): // 12 hour clock 02:55:02 pm
            m_formatters.push_back(details::make_unique<details::r_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('R'): // 24-hour HH:MM time
            m_formatters.push_back(details::make_unique<details::R_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('T'):
        case('X'): // ISO 8601 time format (HH:MM:SS)
            m_formatters.push_back(details::make_unique<details::T_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('z'): // timezone
            m_formatters.push_back(details::make_unique<details::z_formatter<Padder>>(padding));
            m_need_localtime = true;
            break;

        case('P'): // pid
            m_formatters.push_back(details::make_unique<details::pid_formatter<Padder>>(padding));
            break;

        case('^'): // color range start
            m_formatters.push_back(details::make_unique<details::color_start_formatter>(padding));
            break;

        case('$'): // color range end
            m_formatters.push_back(details::make_unique<details::color_stop_formatter>(padding));
            break;

        case('@'): // source location (filename:filenumber)
            m_formatters.push_back(details::make_unique<details::source_location_formatter<Padder>>(padding));
            break;

        case('s'): // short source filename - without directory name
            m_formatters.push_back(details::make_unique<details::short_filename_formatter<Padder>>(padding));
            break;

        case('g'): // full source filename
            m_formatters.push_back(details::make_unique<details::source_filename_formatter<Padder>>(padding));
            break;

        case('#'): // source line number
            m_formatters.push_back(details::make_unique<details::source_linenum_formatter<Padder>>(padding));
            break;

        case('!'): // source funcname
            m_formatters.push_back(details::make_unique<details::source_funcname_formatter<Padder>>(padding));
            break;

        case('%'): // % char
            m_formatters.push_back(details::make_unique<details::ch_formatter>('%'));
            break;

        case('u'): // elapsed time since last log message in nanos
            m_formatters.push_back(details::make_unique<details::elapsed_formatter<Padder, rsl::chrono::nanoseconds>>(padding));
            break;

        case('i'): // elapsed time since last log message in micros
            m_formatters.push_back(details::make_unique<details::elapsed_formatter<Padder, rsl::chrono::microseconds>>(padding));
            break;

        case('o'): // elapsed time since last log message in millis
            m_formatters.push_back(details::make_unique<details::elapsed_formatter<Padder, rsl::chrono::milliseconds>>(padding));
            break;

        case('O'): // elapsed time since last log message in seconds
            m_formatters.push_back(details::make_unique<details::elapsed_formatter<Padder, rsl::chrono::seconds>>(padding));
            break;

        default: // Unknown flag appears as is
            auto unknown_flag = details::make_unique<details::aggregate_formatter>();

            if (!padding.truncate)
            {
                unknown_flag->add_ch('%');
                unknown_flag->add_ch(flag);
                m_formatters.push_back((rsl::move(unknown_flag)));
            }
            // fix issue #1617 (prev char was '!' and should have been treated as funcname flag instead of truncating flag)
            // rexlog::set_pattern("[%10!] %v") => "[      main] some message"
            // rexlog::set_pattern("[%3!!] %v") => "[mai] some message"
            else
            {
                padding.truncate = false;
                m_formatters.push_back(details::make_unique<details::source_funcname_formatter<Padder>>(padding));
                unknown_flag->add_ch(flag);
                m_formatters.push_back((rsl::move(unknown_flag)));
            }

            break;
        }
    }

    //-------------------------------------------------------------------------
    void PatternFormatter::compile_pattern_impl(const rsl::small_stack_string& pattern)
    {
        auto end = pattern.end();
        rsl::unique_ptr<details::aggregate_formatter> user_chars;
        m_formatters.clear();
        for (auto it = pattern.begin(); it != end; ++it)
        {
            if (*it == '%')
            {
                if (user_chars) // append user chars found so far
                {
                    m_formatters.push_back(rsl::move(user_chars));
                }

                ++it;
                auto padding = details::handle_padspec_impl(it, end);

                if (it != end)
                {
                    if (padding.enabled)
                    {
                        handle_flag_impl<details::ScopedPadder>(*it, padding);
                    }
                    else
                    {
                        handle_flag_impl<details::NullScopedPadder>(*it, padding);
                    }
                }
                else
                {
                    break;
                }
            }
            else // chars not following the % sign should be displayed as is
            {
                if (!user_chars)
                {
                    user_chars = details::make_unique<details::aggregate_formatter>();
                }
                user_chars->add_ch(*it);
            }
        }
        if (user_chars) // append raw chars found so far
        {
            m_formatters.push_back(rsl::move(user_chars));
        }
    }
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
// NOLINTEND(readability-identifier-naming)