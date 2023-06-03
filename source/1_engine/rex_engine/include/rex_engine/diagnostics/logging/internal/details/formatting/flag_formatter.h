#pragma once

#include "rex_engine/diagnostics/logging/internal/details/formatting/padding_info.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_std/array.h"

namespace rexlog
{
  namespace details
  {
    //-------------------------------------------------------------------------
    const char* ampm(const tm& t);
    int to12h(const tm& t);

    //-------------------------------------------------------------------------
    struct DayNames
    {
      DayNames()                      = delete;
      DayNames(const DayNames& other) = delete;
      DayNames(DayNames&& other)      = delete;

      DayNames& operator=(const DayNames& other) = delete;
      DayNames& operator=(DayNames&& other)      = delete;

      ~DayNames() = default;

      // Abbreviated weekday name
      static const rsl::array<rsl::string_view, 7> s_days;
      // Full weekday name
      static const rsl::array<rsl::string_view, 7> s_full_days;
    };

    struct MonthNames
    {
      MonthNames()                        = delete;
      MonthNames(const MonthNames& other) = delete;
      MonthNames(MonthNames&& other)      = delete;

      MonthNames& operator=(const MonthNames& other) = delete;
      MonthNames& operator=(MonthNames&& other)      = delete;

      ~MonthNames() = default;

      // Abbreviated month
      static const rsl::array<rsl::string_view, 12> s_months;
      // Full month name
      static const rsl::array<rsl::string_view, 12> s_full_months;
    };

    //-------------------------------------------------------------------------
    class FlagFormatter
    {
    public:
      FlagFormatter() = default;

      explicit FlagFormatter(PaddingInfo padinfo);
      virtual ~FlagFormatter() = default;

      virtual void format(const details::LogMsg& msg, const tm& tmTime, rsl::big_stack_string& dest) = 0;

    protected:
      void set_padding_info(const details::PaddingInfo& padding);
      const PaddingInfo& padding_info() const;

    private:
      PaddingInfo m_padinfo;
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class NameFormatter final : public FlagFormatter
    {
    public:
      explicit NameFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        ScopedPadder p(msg.logger_name().size(), padding_info(), dest);
        fmt_helper::append_string_view(msg.logger_name(), dest);
      }
    };

    //-------------------------------------------------------------------------
    // log level appender
    template <typename ScopedPadder>
    class LevelFormatter final : public FlagFormatter
    {
    public:
      explicit LevelFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        const rsl::string_view level_name = level::to_string_view(msg.level());
        ScopedPadder p(level_name.size(), padding_info(), dest);
        fmt_helper::append_string_view(level_name, dest);
      }
    };

    //-------------------------------------------------------------------------
    // short log level appender
    template <typename ScopedPadder>
    class ShortLevelFormatter final : public FlagFormatter
    {
    public:
      explicit ShortLevelFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        const rsl::string_view level_name {level::to_short_c_str(msg.level())};
        ScopedPadder p(level_name.size(), padding_info(), dest);
        fmt_helper::append_string_view(level_name, dest);
      }
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class a_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit a_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const rsl::string_view field_value {DayNames::s_days[static_cast<count_t>(tmTime.tm_wday)]};
        ScopedPadder p(field_value.size(), padding_info(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class A_Formatter : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit A_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const rsl::string_view field_value {DayNames::s_full_days[static_cast<count_t>(tmTime.tm_wday)]};
        ScopedPadder p(field_value.size(), padding_info(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class b_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit b_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const rsl::string_view field_value {MonthNames::s_months[static_cast<count_t>(tmTime.tm_mon)]};
        ScopedPadder p(field_value.size(), padding_info(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class B_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit B_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const rsl::string_view field_value {MonthNames::s_full_months[static_cast<count_t>(tmTime.tm_mon)]};
        ScopedPadder p(field_value.size(), padding_info(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    //-------------------------------------------------------------------------
    // Date and time representation (Thu Aug 23 15:35:46 2014)
    template <typename ScopedPadder>
    class c_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit c_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 24;
        ScopedPadder p(field_size, padding_info(), dest);

        fmt_helper::append_string_view(rsl::string_view(DayNames::s_days[static_cast<count_t>(tmTime.tm_wday)]), dest);
        dest.push_back(' ');
        fmt_helper::append_string_view(rsl::string_view(MonthNames::s_months[static_cast<count_t>(tmTime.tm_mon)]), dest);
        dest.push_back(' ');
        fmt_helper::append_int(tmTime.tm_mday, dest);
        dest.push_back(' ');
        // time

        fmt_helper::pad2(tmTime.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tmTime.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tmTime.tm_sec, dest);
        dest.push_back(' ');
        fmt_helper::append_int(tmTime.tm_year + 1900, dest);
      }
    };

    //-------------------------------------------------------------------------
    // year - 2 digit
    template <typename ScopedPadder>
    class C_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit C_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad2(tmTime.tm_year % 100, dest);
      }
    };

    //-------------------------------------------------------------------------
    // Short MM/DD/YY date, equivalent to %m/%d/%y 08/23/01
    template <typename ScopedPadder>
    class D_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit D_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 10;
        ScopedPadder p(field_size, padding_info(), dest);

        fmt_helper::pad2(tmTime.tm_mon + 1, dest);
        dest.push_back('/');
        fmt_helper::pad2(tmTime.tm_mday, dest);
        dest.push_back('/');
        fmt_helper::pad2(tmTime.tm_year % 100, dest);
      }
    };

    //-------------------------------------------------------------------------
    // year - 4 digit
    template <typename ScopedPadder>
    class Y_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit Y_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 4;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::append_int(tmTime.tm_year + 1900, dest);
      }
    };

    //-------------------------------------------------------------------------
    // month 1-12
    template <typename ScopedPadder>
    class m_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit m_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad2(tmTime.tm_mon + 1, dest);
      }
    };

    //-------------------------------------------------------------------------
    // day of month 1-31
    template <typename ScopedPadder>
    class d_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit d_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad2(tmTime.tm_mday, dest);
      }
    };

    //-------------------------------------------------------------------------
    // hours in 24 format 0-23
    template <typename ScopedPadder>
    class H_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit H_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad2(tmTime.tm_hour, dest);
      }
    };

    //-------------------------------------------------------------------------
    // hours in 12 format 1-12
    template <typename ScopedPadder>
    class I_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit I_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad2(to12h(tmTime), dest);
      }
    };

    //-------------------------------------------------------------------------
    // minutes 0-59
    template <typename ScopedPadder>
    class M_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit M_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad2(tmTime.tm_min, dest);
      }
    };

    //-------------------------------------------------------------------------
    // seconds 0-59
    template <typename ScopedPadder>
    class S_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit S_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad2(tmTime.tm_sec, dest);
      }
    };

    //-------------------------------------------------------------------------
    // milliseconds
    template <typename ScopedPadder>
    class e_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit e_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        auto millis          = fmt_helper::time_fraction<rsl::chrono::milliseconds>(msg.time());
        const s32 field_size = 3;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
      }
    };

    //-------------------------------------------------------------------------
    // microseconds
    template <typename ScopedPadder>
    class f_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit f_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        auto micros = fmt_helper::time_fraction<rsl::chrono::microseconds>(msg.time());

        const s32 field_size = 6;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad6(static_cast<u32>(micros.count()), dest);
      }
    };

    //-------------------------------------------------------------------------
    // nanoseconds
    template <typename ScopedPadder>
    class F_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit F_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        auto ns              = fmt_helper::time_fraction<rsl::chrono::nanoseconds>(msg.time());
        const s32 field_size = 9;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::pad9(static_cast<u32>(ns.count()), dest);
      }
    };

    //-------------------------------------------------------------------------
    // seconds since epoch
    template <typename ScopedPadder>
    class E_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit E_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 10;
        ScopedPadder p(field_size, padding_info(), dest);
        auto duration = msg.time().time_since_epoch();
        auto seconds  = rsl::chrono::duration_cast<rsl::chrono::seconds>(duration).count();
        fmt_helper::append_int(seconds, dest);
      }
    };

    //-------------------------------------------------------------------------
    // AM/PM
    template <typename ScopedPadder>
    class p_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit p_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 2;
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::append_string_view(rsl::string_view(ampm(tmTime)), dest);
      }
    };

    //-------------------------------------------------------------------------
    // 12 hour clock 02:55:02 pm
    template <typename ScopedPadder>
    class r_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit r_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 11;
        ScopedPadder p(field_size, padding_info(), dest);

        fmt_helper::pad2(to12h(tmTime), dest);
        dest.push_back(':');
        fmt_helper::pad2(tmTime.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tmTime.tm_sec, dest);
        dest.push_back(' ');
        fmt_helper::append_string_view(rsl::string_view(ampm(tmTime)), dest);
      }
    };

    //-------------------------------------------------------------------------
    // 24-hour HH:MM time, equivalent to %H:%M
    template <typename ScopedPadder>
    class R_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit R_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 5;
        ScopedPadder p(field_size, padding_info(), dest);

        fmt_helper::pad2(tmTime.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tmTime.tm_min, dest);
      }
    };

    //-------------------------------------------------------------------------
    // ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S
    template <typename ScopedPadder>
    class T_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit T_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 8;
        ScopedPadder p(field_size, padding_info(), dest);

        fmt_helper::pad2(tmTime.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tmTime.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tmTime.tm_sec, dest);
      }
    };

    //-------------------------------------------------------------------------
    // ISO 8601 offset from UTC in timezone (+-HH:MM)
    template <typename ScopedPadder>
    class z_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit z_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      z_Formatter()                              = default;
      z_Formatter(const z_Formatter&)            = delete;
      ~z_Formatter() override                    = default;
      z_Formatter& operator=(const z_Formatter&) = delete;

      void format(const details::LogMsg& msg, const tm& tmTime, rsl::big_stack_string& dest) override
      {
        const s32 field_size = 6;
        ScopedPadder p(field_size, padding_info(), dest);

        auto total_minutes     = get_cached_offset(msg, tmTime);
        const bool is_negative = total_minutes < 0;
        if(is_negative)
        {
          total_minutes = -total_minutes;
          dest.push_back('-');
        }
        else
        {
          dest.push_back('+');
        }

        fmt_helper::pad2(total_minutes / 60, dest); // hours
        dest.push_back(':');
        fmt_helper::pad2(total_minutes % 60, dest); // minutes
      }

    private:
      int get_cached_offset(const LogMsg& msg, const tm& tmTime)
      {
        // refresh every 10 seconds
        if(msg.time() - m_last_update >= rsl::chrono::seconds(10))
        {
          m_offset_minutes = os::utc_minutes_offset(tmTime);
          m_last_update    = msg.time();
        }
        return m_offset_minutes;
      }

    private:
      log_clock::time_point m_last_update {rsl::chrono::seconds(0)};
      int m_offset_minutes {0};
    };

    //-------------------------------------------------------------------------
    // Thread id
    template <typename ScopedPadder>
    class t_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit t_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        const auto field_size = ScopedPadder::count_digits(msg.thread_id());
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::append_int(msg.thread_id(), dest);
      }
    };

    //-------------------------------------------------------------------------
    // Current pid
    template <typename ScopedPadder>
    class ProcessIdentifierFormatter final : public FlagFormatter
    {
    public:
      explicit ProcessIdentifierFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        const auto pid  = static_cast<uint32_t>(details::os::pid());
        auto field_size = ScopedPadder::count_digits(pid);
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::append_int(pid, dest);
      }
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class v_Formatter final : public FlagFormatter // NOLINT(readability-identifier-naming)
    {
    public:
      explicit v_Formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        ScopedPadder p(msg.payload().size(), padding_info(), dest);
        fmt_helper::append_string_view(msg.payload(), dest);
      }
    };

    //-------------------------------------------------------------------------
    // print source location
    template <typename ScopedPadder>
    class SourceLocationFormatter final : public FlagFormatter
    {
    public:
      explicit SourceLocationFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        if(!msg.source().file_name().empty())
        {
          ScopedPadder p(0, padding_info(), dest);
          return;
        }

        count_t text_size = 0;
        if(padding_info().enabled)
        {
          // calc text size for padding based on "filename:line"
          text_size = msg.source().file_name().length() + ScopedPadder::count_digits(msg.source().line()) + 1;
        }
        else
        {
          text_size = 0;
        }

        ScopedPadder p(text_size, padding_info(), dest);
        fmt_helper::append_string_view(msg.source().file_name(), dest);
        dest.push_back(':');
        fmt_helper::append_int(msg.source().line(), dest);
      }
    };

    //-------------------------------------------------------------------------
    // print source filename
    template <typename ScopedPadder>
    class SourceFileNameFormatter final : public FlagFormatter
    {
    public:
      explicit SourceFileNameFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        if(!msg.source().file_name().empty())
        {
          ScopedPadder p(0, padding_info(), dest);
          return;
        }
        const count_t text_size = padding_info().enabled ? msg.source().file_name().length() : 0;
        ScopedPadder p(text_size, padding_info(), dest);
        fmt_helper::append_string_view(msg.source().file_name(), dest);
      }
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class ShortFileNameFormatter final : public FlagFormatter
    {
    public:
      explicit ShortFileNameFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4127) // consider using 'if constexpr' instead
#endif                            // _MSC_VER
      static const char* basename(const char* filename)
      {
        // if the size is 2 (1 character + null terminator) we can use the more efficient strrchr
        // the branch will be elided by optimizations
        if(sizeof(os::g_folder_seps) == 2)
        {
          const char* rv = rsl::strrchr(filename, os::g_folder_seps[0]);
          return rv != nullptr ? rv + 1 : filename;
        }
        else
        {
          const rsl::reverse_iterator<const char*> begin(filename + rsl::strlen(filename));
          const rsl::reverse_iterator<const char*> end(filename);

          const auto it = begin + rsl::find_first_of(begin, rsl::strlen(filename), os::g_folder_seps.data(), os::g_folder_seps.size());
          return it != end ? it.base() : filename;
        }
      }
#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        if(!msg.source().file_name().empty())
        {
          ScopedPadder p(0, padding_info(), dest);
          return;
        }
        const auto filename     = basename(msg.source().file_name().data()); // NOLINT(readability-qualified-auto)
        const count_t text_size = padding_info().enabled ? rsl::char_traits<char>::length(filename) : 0;
        ScopedPadder p(text_size, padding_info(), dest);
        fmt_helper::append_string_view(rsl::string_view(filename), dest);
      }
    };

    //-------------------------------------------------------------------------
    template <typename ScopedPadder>
    class SourceLineNumberFormatter final : public FlagFormatter
    {
    public:
      explicit SourceLineNumberFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        if(!msg.source().file_name().empty())
        {
          ScopedPadder p(0, padding_info(), dest);
          return;
        }

        auto field_size = ScopedPadder::count_digits(msg.source().line());
        ScopedPadder p(field_size, padding_info(), dest);
        fmt_helper::append_int(msg.source().line(), dest);
      }
    };

    //-------------------------------------------------------------------------
    // print source funcname
    template <typename ScopedPadder>
    class SourceFunctionNameFormatter final : public FlagFormatter
    {
    public:
      explicit SourceFunctionNameFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        if(!msg.source().file_name().empty())
        {
          ScopedPadder p(0, padding_info(), dest);
          return;
        }
        const count_t text_size = padding_info().enabled ? msg.source().function_name().length() : 0;
        ScopedPadder p(text_size, padding_info(), dest);

        fmt_helper::append_string_view(msg.source().function_name(), dest);
      }
    };

    //-------------------------------------------------------------------------
    // print elapsed time since last message
    template <typename ScopedPadder, typename Units>
    class ElapsedFormatter final : public FlagFormatter
    {
    public:
      using DurationUnits = Units;

      explicit ElapsedFormatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
          , m_last_message_time(log_clock::now())
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override
      {
        auto delta          = (rsl::max)(msg.time() - m_last_message_time, log_clock::duration::zero());
        auto delta_units    = rsl::chrono::duration_cast<DurationUnits>(delta);
        m_last_message_time = msg.time();
        auto delta_count    = static_cast<s32>(delta_units.count());
        auto n_digits       = static_cast<s32>(ScopedPadder::count_digits(delta_count));
        ScopedPadder p(n_digits, padding_info(), dest);
        fmt_helper::append_int(delta_count, dest);
      }

    private:
      log_clock::time_point m_last_message_time;
    };

    //-------------------------------------------------------------------------
    class CharacterFormatter final : public FlagFormatter
    {
    public:
      explicit CharacterFormatter(char ch);

      void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, rsl::big_stack_string& dest) override;

    private:
      char m_character;
    };

    //-------------------------------------------------------------------------
    // aggregate user chars to display as is
    class AggregateFormatter final : public FlagFormatter
    {
    public:
      AggregateFormatter() = default;

      void add_ch(char ch);
      void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, rsl::big_stack_string& dest) override;

    private:
      rex::DebugString m_string;
    };

    //-------------------------------------------------------------------------
    // mark the color range. expect it to be in the form of "%^colored text%$"
    class ColorStartFormatter final : public FlagFormatter
    {
    public:
      explicit ColorStartFormatter(PaddingInfo padinfo);

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override;
    };

    //-------------------------------------------------------------------------
    class ColorStopFormatter final : public FlagFormatter
    {
    public:
      explicit ColorStopFormatter(PaddingInfo padinfo);

      void format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest) override;
    };

    //-------------------------------------------------------------------------
    // Full info formatter
    // pattern: [%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%s:%#] %v
    class FullFormatter final : public FlagFormatter
    {
    public:
      explicit FullFormatter(PaddingInfo padinfo);

      void format(const details::LogMsg& msg, const tm& tmTime, rsl::big_stack_string& dest) override;

    private:
      rsl::chrono::seconds m_cache_timestamp {0};
      rsl::big_stack_string m_cached_datetime;
    };
  } // namespace details
} // namespace rexlog