

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/fmt_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/formatter.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

// NOLINTBEGIN(readability-identifier-naming)
// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  namespace details
  {

    ///////////////////////////////////////////////////////////////////////
    // name & level pattern appender
    ///////////////////////////////////////////////////////////////////////

    class ScopedPadder
    {
    public:
      ScopedPadder(size_t wrapped_size, const PaddingInfo& padinfo, memory_buf_t& dest)
          : m_padinfo(&padinfo)
          , m_dest(&dest)
          , m_remaining_pag(static_cast<long>(padinfo.width) - static_cast<long>(wrapped_size))
      {
        if(m_remaining_pag <= 0)
        {
          return;
        }

        if(m_padinfo->side == PaddingInfo::PadSide::Left)
        {
          pad_it(m_remaining_pag);
          m_remaining_pag = 0;
        }
        else if(m_padinfo->side == PaddingInfo::PadSide::Center)
        {
          auto half_pad = m_remaining_pag / 2;
          auto reminder = m_remaining_pag & 1u;
          pad_it(static_cast<long>(half_pad));
          m_remaining_pag = static_cast<unsigned long>(half_pad + reminder); // for the Right side
        }
      }

      ScopedPadder(const ScopedPadder&) = delete;
      ScopedPadder(ScopedPadder&&)      = delete;
      ~ScopedPadder()
      {
        if(m_remaining_pag >= 0)
        {
          pad_it(m_remaining_pag);
        }
        else if(m_padinfo->truncate)
        {
          const unsigned long new_size = m_dest->size() + m_remaining_pag;
          m_dest->resize(static_cast<count_t>(new_size));
        }
      }

      ScopedPadder& operator=(const ScopedPadder&) = delete;
      ScopedPadder& operator=(ScopedPadder&&)      = delete;

      template <typename T>
      static unsigned int count_digits(T n)
      {
        return fmt_helper::count_digits(n);
      }

    private:
      void pad_it(unsigned long count)
      {
        fmt_helper::append_string_view(string_view_t(m_spaces.data(), static_cast<count_t>(count)), *m_dest);
      }

      const PaddingInfo* m_padinfo;
      memory_buf_t* m_dest;
      unsigned long m_remaining_pag;
      string_view_t m_spaces {"                                                                ", 64};
    };

    struct null_scoped_padder
    {
      null_scoped_padder(size_t /*wrapped_size*/, const PaddingInfo& /*padinfo*/, memory_buf_t& /*dest*/) {}

      template <typename T>
      static unsigned int count_digits(T /* number */)
      {
        return 0;
      }
    };

    template <typename ScopedPadder>
    class name_formatter final : public FlagFormatter
    {
    public:
      explicit name_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        ScopedPadder p(msg.logger_name.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(msg.logger_name, dest);
      }
    };

    // log level appender
    template <typename ScopedPadder>
    class level_formatter final : public FlagFormatter
    {
    public:
      explicit level_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        const string_view_t& level_name = level::to_string_view(msg.level);
        ScopedPadder p(level_name.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(level_name, dest);
      }
    };

    // short log level appender
    template <typename ScopedPadder>
    class short_level_formatter final : public FlagFormatter
    {
    public:
      explicit short_level_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        const string_view_t level_name {level::to_short_c_str(msg.level)};
        ScopedPadder p(level_name.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(level_name, dest);
      }
    };

    ///////////////////////////////////////////////////////////////////////
    // Date time pattern appenders
    ///////////////////////////////////////////////////////////////////////

    static const char* ampm(const tm& t)
    {
      return t.tm_hour >= 12 ? "PM" : "AM";
    }

    static int to12h(const tm& t)
    {
      return t.tm_hour > 12 ? t.tm_hour - 12 : t.tm_hour;
    }

    // Abbreviated weekday name
    static rsl::array<const char*, 7> days {{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    template <typename ScopedPadder>
    class a_formatter final : public FlagFormatter
    {
    public:
      explicit a_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const string_view_t field_value {days[static_cast<count_t>(tm_time.tm_wday)]};
        ScopedPadder p(field_value.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    // Full weekday name
    static rsl::array<const char*, 7> full_days {{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    template <typename ScopedPadder>
    class A_formatter : public FlagFormatter
    {
    public:
      explicit A_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const string_view_t field_value {full_days[static_cast<count_t>(tm_time.tm_wday)]};
        ScopedPadder p(field_value.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    // Abbreviated month
    static const rsl::array<const char*, 12> months {{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    template <typename ScopedPadder>
    class b_formatter final : public FlagFormatter
    {
    public:
      explicit b_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const string_view_t field_value {months[static_cast<count_t>(tm_time.tm_mon)]};
        ScopedPadder p(field_value.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    // Full month name
    static const rsl::array<const char*, 12> full_months {{"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"}}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    template <typename ScopedPadder>
    class B_formatter final : public FlagFormatter
    {
    public:
      explicit B_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const string_view_t field_value {full_months[static_cast<count_t>(tm_time.tm_mon)]};
        ScopedPadder p(field_value.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(field_value, dest);
      }
    };

    // Date and time representation (Thu Aug 23 15:35:46 2014)
    template <typename ScopedPadder>
    class c_formatter final : public FlagFormatter
    {
    public:
      explicit c_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 24;
        ScopedPadder p(field_size, paddinginfo(), dest);

        fmt_helper::append_string_view(string_view_t(days[static_cast<count_t>(tm_time.tm_wday)]), dest);
        dest.push_back(' ');
        fmt_helper::append_string_view(string_view_t(months[static_cast<count_t>(tm_time.tm_mon)]), dest);
        dest.push_back(' ');
        fmt_helper::append_int(tm_time.tm_mday, dest);
        dest.push_back(' ');
        // time

        fmt_helper::pad2(tm_time.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_sec, dest);
        dest.push_back(' ');
        fmt_helper::append_int(tm_time.tm_year + 1900, dest);
      }
    };

    // year - 2 digit
    template <typename ScopedPadder>
    class C_formatter final : public FlagFormatter
    {
    public:
      explicit C_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad2(tm_time.tm_year % 100, dest);
      }
    };

    // Short MM/DD/YY date, equivalent to %m/%d/%y 08/23/01
    template <typename ScopedPadder>
    class D_formatter final : public FlagFormatter
    {
    public:
      explicit D_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 10;
        ScopedPadder p(field_size, paddinginfo(), dest);

        fmt_helper::pad2(tm_time.tm_mon + 1, dest);
        dest.push_back('/');
        fmt_helper::pad2(tm_time.tm_mday, dest);
        dest.push_back('/');
        fmt_helper::pad2(tm_time.tm_year % 100, dest);
      }
    };

    // year - 4 digit
    template <typename ScopedPadder>
    class Y_formatter final : public FlagFormatter
    {
    public:
      explicit Y_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 4;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::append_int(tm_time.tm_year + 1900, dest);
      }
    };

    // month 1-12
    template <typename ScopedPadder>
    class m_formatter final : public FlagFormatter
    {
    public:
      explicit m_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad2(tm_time.tm_mon + 1, dest);
      }
    };

    // day of month 1-31
    template <typename ScopedPadder>
    class d_formatter final : public FlagFormatter
    {
    public:
      explicit d_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad2(tm_time.tm_mday, dest);
      }
    };

    // hours in 24 format 0-23
    template <typename ScopedPadder>
    class H_formatter final : public FlagFormatter
    {
    public:
      explicit H_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad2(tm_time.tm_hour, dest);
      }
    };

    // hours in 12 format 1-12
    template <typename ScopedPadder>
    class I_formatter final : public FlagFormatter
    {
    public:
      explicit I_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad2(to12h(tm_time), dest);
      }
    };

    // minutes 0-59
    template <typename ScopedPadder>
    class M_formatter final : public FlagFormatter
    {
    public:
      explicit M_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad2(tm_time.tm_min, dest);
      }
    };

    // seconds 0-59
    template <typename ScopedPadder>
    class S_formatter final : public FlagFormatter
    {
    public:
      explicit S_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad2(tm_time.tm_sec, dest);
      }
    };

    // milliseconds
    template <typename ScopedPadder>
    class e_formatter final : public FlagFormatter
    {
    public:
      explicit e_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        auto millis             = fmt_helper::time_fraction<rsl::chrono::milliseconds>(msg.time);
        const size_t field_size = 3;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
      }
    };

    // microseconds
    template <typename ScopedPadder>
    class f_formatter final : public FlagFormatter
    {
    public:
      explicit f_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        auto micros = fmt_helper::time_fraction<rsl::chrono::microseconds>(msg.time);

        const size_t field_size = 6;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad6(static_cast<size_t>(micros.count()), dest);
      }
    };

    // nanoseconds
    template <typename ScopedPadder>
    class F_formatter final : public FlagFormatter
    {
    public:
      explicit F_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        auto ns                 = fmt_helper::time_fraction<rsl::chrono::nanoseconds>(msg.time);
        const size_t field_size = 9;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::pad9(static_cast<size_t>(ns.count()), dest);
      }
    };

    // seconds since epoch
    template <typename ScopedPadder>
    class E_formatter final : public FlagFormatter
    {
    public:
      explicit E_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        const size_t field_size = 10;
        ScopedPadder p(field_size, paddinginfo(), dest);
        auto duration = msg.time.time_since_epoch();
        auto seconds  = rsl::chrono::duration_cast<rsl::chrono::seconds>(duration).count();
        fmt_helper::append_int(seconds, dest);
      }
    };

    // AM/PM
    template <typename ScopedPadder>
    class p_formatter final : public FlagFormatter
    {
    public:
      explicit p_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 2;
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::append_string_view(string_view_t(ampm(tm_time)), dest);
      }
    };

    // 12 hour clock 02:55:02 pm
    template <typename ScopedPadder>
    class r_formatter final : public FlagFormatter
    {
    public:
      explicit r_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 11;
        ScopedPadder p(field_size, paddinginfo(), dest);

        fmt_helper::pad2(to12h(tm_time), dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_sec, dest);
        dest.push_back(' ');
        fmt_helper::append_string_view(string_view_t(ampm(tm_time)), dest);
      }
    };

    // 24-hour HH:MM time, equivalent to %H:%M
    template <typename ScopedPadder>
    class R_formatter final : public FlagFormatter
    {
    public:
      explicit R_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 5;
        ScopedPadder p(field_size, paddinginfo(), dest);

        fmt_helper::pad2(tm_time.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
      }
    };

    // ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S
    template <typename ScopedPadder>
    class T_formatter final : public FlagFormatter
    {
    public:
      explicit T_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 8;
        ScopedPadder p(field_size, paddinginfo(), dest);

        fmt_helper::pad2(tm_time.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_sec, dest);
      }
    };

    // ISO 8601 offset from UTC in timezone (+-HH:MM)
    template <typename ScopedPadder>
    class z_formatter final : public FlagFormatter
    {
    public:
      explicit z_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      z_formatter()                              = default;
      z_formatter(const z_formatter&)            = delete;
      ~z_formatter() override                    = default;
      z_formatter& operator=(const z_formatter&) = delete;

      void format(const details::LogMsg& msg, const tm& tm_time, memory_buf_t& dest) override
      {
        const size_t field_size = 6;
        ScopedPadder p(field_size, paddinginfo(), dest);

        auto total_minutes     = get_cached_offset(msg, tm_time);
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
      log_clock::time_point last_update_ {rsl::chrono::seconds(0)};
      int offset_minutes_ {0};

      int get_cached_offset(const LogMsg& msg, const tm& tm_time)
      {
        // refresh every 10 seconds
        if(msg.time - last_update_ >= rsl::chrono::seconds(10))
        {
          offset_minutes_ = os::utc_minutes_offset(tm_time);
          last_update_    = msg.time;
        }
        return offset_minutes_;
      }
    };

    // Thread id
    template <typename ScopedPadder>
    class t_formatter final : public FlagFormatter
    {
    public:
      explicit t_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        const auto field_size = ScopedPadder::count_digits(msg.thread_id);
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::append_int(msg.thread_id, dest);
      }
    };

    // Current pid
    template <typename ScopedPadder>
    class pid_formatter final : public FlagFormatter
    {
    public:
      explicit pid_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, memory_buf_t& dest) override
      {
        const auto pid  = static_cast<uint32_t>(details::os::pid());
        auto field_size = ScopedPadder::count_digits(pid);
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::append_int(pid, dest);
      }
    };

    template <typename ScopedPadder>
    class v_formatter final : public FlagFormatter
    {
    public:
      explicit v_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        ScopedPadder p(msg.payload.size(), paddinginfo(), dest);
        fmt_helper::append_string_view(msg.payload, dest);
      }
    };

    class ch_formatter final : public FlagFormatter
    {
    public:
      explicit ch_formatter(char ch)
          : ch_(ch)
      {
      }

      void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, memory_buf_t& dest) override
      {
        dest.push_back(ch_);
      }

    private:
      char ch_;
    };

    // aggregate user chars to display as is
    class aggregate_formatter final : public FlagFormatter
    {
    public:
      aggregate_formatter() = default;

      void add_ch(char ch)
      {
        str_ += ch;
      }
      void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, memory_buf_t& dest) override
      {
        fmt_helper::append_string_view(str_, dest);
      }

    private:
      rex::DebugString str_;
    };

    // mark the color range. expect it to be in the form of "%^colored text%$"
    class color_start_formatter final : public FlagFormatter
    {
    public:
      explicit color_start_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        msg.color_range_start = dest.size();
      }
    };

    class color_stop_formatter final : public FlagFormatter
    {
    public:
      explicit color_stop_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        msg.color_range_end = dest.size();
      }
    };

    // print source location
    template <typename ScopedPadder>
    class source_location_formatter final : public FlagFormatter
    {
    public:
      explicit source_location_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        if(msg.source.empty())
        {
          ScopedPadder p(0, paddinginfo(), dest);
          return;
        }

        count_t text_size = 0;
        if(paddinginfo().enabled())
        {
          // calc text size for padding based on "filename:line"
          text_size = rsl::char_traits<char>::length(msg.source.filename) + ScopedPadder::count_digits(msg.source.line) + 1;
        }
        else
        {
          text_size = 0;
        }

        ScopedPadder p(text_size, paddinginfo(), dest);
        fmt_helper::append_string_view(string_view_t(msg.source.filename), dest);
        dest.push_back(':');
        fmt_helper::append_int(msg.source.line, dest);
      }
    };

    // print source filename
    template <typename ScopedPadder>
    class source_filename_formatter final : public FlagFormatter
    {
    public:
      explicit source_filename_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        if(msg.source.empty())
        {
          ScopedPadder p(0, paddinginfo(), dest);
          return;
        }
        const count_t text_size = paddinginfo().enabled() ? rsl::char_traits<char>::length(msg.source.filename) : 0;
        ScopedPadder p(text_size, paddinginfo(), dest);
        fmt_helper::append_string_view(string_view_t(msg.source.filename), dest);
      }
    };

    template <typename ScopedPadder>
    class short_filename_formatter final : public FlagFormatter
    {
    public:
      explicit short_filename_formatter(PaddingInfo padinfo)
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
        if(sizeof(os::folder_seps) == 2)
        {
          const char* rv = rsl::strrchr(filename, os::folder_seps[0]);
          return rv != nullptr ? rv + 1 : filename;
        }
        else
        {
          const rsl::reverse_iterator<const char*> begin(filename + rsl::strlen(filename));
          const rsl::reverse_iterator<const char*> end(filename);

          const auto it = begin + rsl::find_first_of(begin, rsl::strlen(filename), os::folder_seps.data(), os::folder_seps.size());
          return it != end ? it.base() : filename;
        }
      }
#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        if(msg.source.empty())
        {
          ScopedPadder p(0, paddinginfo(), dest);
          return;
        }
        const auto filename     = basename(msg.source.filename); // NOLINT(readability-qualified-auto)
        const count_t text_size = paddinginfo().enabled() ? rsl::char_traits<char>::length(filename) : 0;
        ScopedPadder p(text_size, paddinginfo(), dest);
        fmt_helper::append_string_view(rsl::string_view(filename), dest);
      }
    };

    template <typename ScopedPadder>
    class source_linenum_formatter final : public FlagFormatter
    {
    public:
      explicit source_linenum_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        if(msg.source.empty())
        {
          ScopedPadder p(0, paddinginfo(), dest);
          return;
        }

        auto field_size = ScopedPadder::count_digits(msg.source.line);
        ScopedPadder p(field_size, paddinginfo(), dest);
        fmt_helper::append_int(msg.source.line, dest);
      }
    };

    // print source funcname
    template <typename ScopedPadder>
    class source_funcname_formatter final : public FlagFormatter
    {
    public:
      explicit source_funcname_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        if(msg.source.empty())
        {
          ScopedPadder p(0, paddinginfo(), dest);
          return;
        }
        const count_t text_size = paddinginfo().enabled() ? rsl::char_traits<char>::length(msg.source.funcname) : 0;
        ScopedPadder p(text_size, paddinginfo(), dest);
        fmt_helper::append_string_view(string_view_t(msg.source.funcname), dest);
      }
    };

    // print elapsed time since last message
    template <typename ScopedPadder, typename Units>
    class elapsed_formatter final : public FlagFormatter
    {
    public:
      using DurationUnits = Units;

      explicit elapsed_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
          , last_message_time_(log_clock::now())
      {
      }

      void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override
      {
        auto delta         = (rsl::max)(msg.time - last_message_time_, log_clock::duration::zero());
        auto delta_units   = rsl::chrono::duration_cast<DurationUnits>(delta);
        last_message_time_ = msg.time;
        auto delta_count   = static_cast<size_t>(delta_units.count());
        auto n_digits      = static_cast<size_t>(ScopedPadder::count_digits(delta_count));
        ScopedPadder p(n_digits, paddinginfo(), dest);
        fmt_helper::append_int(delta_count, dest);
      }

    private:
      log_clock::time_point last_message_time_;
    };

    // Full info formatter
    // pattern: [%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%s:%#] %v
    class full_formatter final : public FlagFormatter
    {
    public:
      explicit full_formatter(PaddingInfo padinfo)
          : FlagFormatter(padinfo)
      {
      }

      void format(const details::LogMsg& msg, const tm& tm_time, memory_buf_t& dest) override
      {
        using rsl::chrono::duration_cast;
        using rsl::chrono::milliseconds;
        using rsl::chrono::seconds;

        // cache the date/time part for the next second.
        auto duration = msg.time.time_since_epoch();
        auto secs     = duration_cast<seconds>(duration);

        if(cache_timestamp_ != secs || cached_datetime_.empty())
        {
          cached_datetime_.clear();
          cached_datetime_.push_back('[');
          fmt_helper::append_int(tm_time.tm_year + 1900, cached_datetime_);
          cached_datetime_.push_back('-');

          fmt_helper::pad2(tm_time.tm_mon + 1, cached_datetime_);
          cached_datetime_.push_back('-');

          fmt_helper::pad2(tm_time.tm_mday, cached_datetime_);
          cached_datetime_.push_back(' ');

          fmt_helper::pad2(tm_time.tm_hour, cached_datetime_);
          cached_datetime_.push_back(':');

          fmt_helper::pad2(tm_time.tm_min, cached_datetime_);
          cached_datetime_.push_back(':');

          fmt_helper::pad2(tm_time.tm_sec, cached_datetime_);
          cached_datetime_.push_back('.');

          cache_timestamp_ = secs;
        }
        dest.append(cached_datetime_.begin(), cached_datetime_.end());

        auto millis = fmt_helper::time_fraction<milliseconds>(msg.time);
        fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
        dest.push_back(']');
        dest.push_back(' ');

        // append logger name if exists
        if(!msg.logger_name.empty())
        {
          dest.push_back('[');
          fmt_helper::append_string_view(string_view_t(msg.logger_name), dest);
          dest.push_back(']');
          dest.push_back(' ');
        }

        dest.push_back('[');
        // wrap the level name with color
        msg.color_range_start = dest.size();
        // fmt_helper::append_string_view(level::to_c_str(msg.level), dest);
        fmt_helper::append_string_view(level::to_string_view(msg.level), dest);
        msg.color_range_end = dest.size();
        dest.push_back(']');
        dest.push_back(' ');

        // add source location if present
        if(!msg.source.empty())
        {
          dest.push_back('[');
          const char* filename = details::short_filename_formatter<details::null_scoped_padder>::basename(msg.source.filename);
          fmt_helper::append_string_view(rsl::string_view(filename), dest);
          dest.push_back(':');
          fmt_helper::append_int(msg.source.line, dest);
          dest.push_back(']');
          dest.push_back(' ');
        }
        // fmt_helper::append_string_view(msg.msg(), dest);
        fmt_helper::append_string_view(string_view_t(msg.payload), dest);
      }

    private:
      rsl::chrono::seconds cache_timestamp_ {0};
      memory_buf_t cached_datetime_;
    };

  } // namespace details

  REXLOG_INLINE PatternFormatter::PatternFormatter(rsl::string_view pattern, PatternTimeType timeType, rex::DebugString eol, custom_flags customUserFlags)
      : m_pattern(pattern)
      , m_eol(rsl::move(eol))
      , m_pattern_time_type(timeType)
      , m_need_localtime(false)
      , m_last_log_secs(0)
      , m_custom_handlers(rsl::move(customUserFlags))
  {
    rsl::memset(&m_cached_tm, 0, sizeof(m_cached_tm));
    compile_pattern_impl(m_pattern);
  }

  // use by default full formatter for if pattern is not given
  REXLOG_INLINE PatternFormatter::PatternFormatter(PatternTimeType timeType, rex::DebugString eol)
      : m_pattern("%+")
      , m_eol(rsl::move(eol))
      , m_pattern_time_type(timeType)
      , m_need_localtime(true)
      , m_last_log_secs(0)
  {
    rsl::memset(&m_cached_tm, 0, sizeof(m_cached_tm));
    m_formatters.push_back(details::make_unique<details::full_formatter>(details::PaddingInfo {}));
  }

  REXLOG_INLINE rsl::unique_ptr<formatter> PatternFormatter::clone() const
  {
    custom_flags cloned_custom_formatters;
    for(const auto& it: m_custom_handlers)
    {
      cloned_custom_formatters[it.key] = it.value->clone();
    }
    auto cloned = details::make_unique<PatternFormatter>(m_pattern, m_pattern_time_type, m_eol, rsl::move(cloned_custom_formatters));
    cloned->need_localtime(m_need_localtime);
#if defined(__GNUC__) && __GNUC__ < 5
    return rsl::move(cloned);
#else
    return cloned;
#endif
  }

  REXLOG_INLINE void PatternFormatter::format(const details::LogMsg& msg, memory_buf_t& dest)
  {
    if(m_need_localtime)
    {
      const auto secs = rsl::chrono::duration_cast<rsl::chrono::seconds>(msg.time.time_since_epoch());
      if(secs != m_last_log_secs)
      {
        m_cached_tm     = get_time_impl(msg);
        m_last_log_secs = secs;
      }
    }

    for(auto& f: m_formatters)
    {
      f->format(msg, m_cached_tm, dest);
    }
    // write eol
    details::fmt_helper::append_string_view(m_eol, dest);
  }

  REXLOG_INLINE void PatternFormatter::set_pattern(rsl::string_view pattern)
  {
    m_pattern        = rex::DebugString(pattern);
    m_need_localtime = false;
    compile_pattern_impl(m_pattern);
  }

  REXLOG_INLINE void PatternFormatter::need_localtime(bool need)
  {
    m_need_localtime = need;
  }

  REXLOG_INLINE tm PatternFormatter::get_time_impl(const details::LogMsg& msg)
  {
    if(m_pattern_time_type == PatternTimeType::Local)
    {
      return details::os::localtime(log_clock::to_time_t(msg.time));
    }
    return details::os::gmtime(log_clock::to_time_t(msg.time));
  }

  template <typename Padder>
  REXLOG_INLINE void PatternFormatter::handle_flag_impl(char flag, details::PaddingInfo padding)
  {
    // process custom flags
    auto it = m_custom_handlers.find(flag);
    if(it != m_custom_handlers.end())
    {
      auto custom_handler = it->value->clone();
      custom_handler->set_padding_info(padding);
      m_formatters.push_back(rsl::move(custom_handler));
      return;
    }

    // process built-in flags
    switch(flag)
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

        if(!padding.truncate)
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

  // Extract given pad spec (e.g. %8X, %=8X, %-8!X, %8!X, %=8!X, %-8!X, %+8!X)
  // Advance the given it pass the end of the padding spec found (if any)
  // Return padding.
  REXLOG_INLINE details::PaddingInfo PatternFormatter::handle_padspec_impl(rex::DebugString::const_iterator& it, rex::DebugString::const_iterator end)
  {
    using details::PaddingInfo;
    using details::ScopedPadder;
    const size_t max_width = 64;
    if(it == end)
    {
      return PaddingInfo {};
    }

    PaddingInfo::PadSide side = PaddingInfo::PadSide::Center;
    switch(*it)
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

    if(it == end || !rsl::is_digit(static_cast<unsigned char>(*it)))
    {
      return PaddingInfo {}; // no padding if no digit found here
    }

    auto width = static_cast<size_t>(*it) - '0';
    for(++it; it != end && rsl::is_digit(static_cast<unsigned char>(*it)); ++it)
    {
      auto digit = static_cast<size_t>(*it) - '0';
      width      = width * 10 + digit;
    }

    // search for the optional truncate marker '!'
    bool truncate = false;
    if(it != end && *it == '!')
    {
      truncate = true;
      ++it;
    }
    else
    {
      truncate = false;
    }
    return details::PaddingInfo {rsl::min<size_t>(width, max_width), side, truncate};
  }

  REXLOG_INLINE void PatternFormatter::compile_pattern_impl(rsl::string_view pattern)
  {
    auto end = pattern.end();
    rsl::unique_ptr<details::aggregate_formatter> user_chars;
    m_formatters.clear();
    for(auto it = pattern.begin(); it != end; ++it)
    {
      if(*it == '%')
      {
        if(user_chars) // append user chars found so far
        {
          m_formatters.push_back(rsl::move(user_chars));
        }

        ++it;
        auto padding = handle_padspec_impl(it, end);

        if(it != end)
        {
          if(padding.enabled())
          {
            handle_flag_impl<details::ScopedPadder>(*it, padding);
          }
          else
          {
            handle_flag_impl<details::null_scoped_padder>(*it, padding);
          }
        }
        else
        {
          break;
        }
      }
      else // chars not following the % sign should be displayed as is
      {
        if(!user_chars)
        {
          user_chars = details::make_unique<details::aggregate_formatter>();
        }
        user_chars->add_ch(*it);
      }
    }
    if(user_chars) // append raw chars found so far
    {
      m_formatters.push_back(rsl::move(user_chars));
    }
  }
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
// NOLINTEND(readability-identifier-naming)