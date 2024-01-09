#include "rex_engine/diagnostics/logging/internal/details/formatting/flag_formatter.h"

// IWYU pragma: no_include <built-in>

#include "rex_engine/diagnostics/logging/internal/details/fmt_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/formatting/padder.h"

namespace rexlog
{
  namespace details
  {
    ///////////////////////////////////////////////////////////////////////
    // Time information
    ///////////////////////////////////////////////////////////////////////
    const rsl::array<rsl::string_view, 7> DayNames::s_days           = {{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}};
    const rsl::array<rsl::string_view, 7> DayNames::s_full_days      = {{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}};
    const rsl::array<rsl::string_view, 12> MonthNames::s_months      = {{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}};
    const rsl::array<rsl::string_view, 12> MonthNames::s_full_months = {{"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"}};

    ///////////////////////////////////////////////////////////////////////
    // Date time pattern appenders
    ///////////////////////////////////////////////////////////////////////

    const char* ampm(const tm& t)
    {
      return t.tm_hour >= 12 ? "PM" : "AM";
    }

    int to12h(const tm& t)
    {
      return t.tm_hour > 12 ? t.tm_hour - 12 : t.tm_hour;
    }

    ///////////////////////////////////////////////////////////////////////
    // FlagFormatter
    ///////////////////////////////////////////////////////////////////////
    FlagFormatter::FlagFormatter(PaddingInfo padinfo)
        : m_padinfo(padinfo)
    {
      // Nothing to implement
    }

    void FlagFormatter::set_padding_info(const details::PaddingInfo& padding)
    {
      m_padinfo = padding;
    }

    const PaddingInfo& FlagFormatter::padding_info() const
    {
      return m_padinfo;
    }

    ///////////////////////////////////////////////////////////////////////
    // ch formatter
    ///////////////////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    CharacterFormatter::CharacterFormatter(char ch)
        : m_character(ch)
    {
    }

    //-------------------------------------------------------------------------
    void CharacterFormatter::format(const details::LogMsg& /*unused*/, const tm& /*unused*/, rsl::big_stack_string& dest)
    {
      dest.push_back(m_character);
    }

    ///////////////////////////////////////////////////////////////////////
    // aggregate formatter
    ///////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    void AggregateFormatter::add_ch(char ch)
    {
      m_string += ch;
    }

    //-------------------------------------------------------------------------
    void AggregateFormatter::format(const details::LogMsg& /*unused*/, const tm& /*unused*/, rsl::big_stack_string& dest)
    {
      fmt_helper::append_string_view(m_string, dest);
    }

    ///////////////////////////////////////////////////////////////////////
    // color start formatter
    ///////////////////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    ColorStartFormatter::ColorStartFormatter(PaddingInfo padinfo)
        : FlagFormatter(padinfo)
    {
    }

    //-------------------------------------------------------------------------
    void ColorStartFormatter::format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest)
    {
      msg.m_color_range_start = dest.size();
    }

    ///////////////////////////////////////////////////////////////////////
    // color stop formatter
    ///////////////////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    ColorStopFormatter::ColorStopFormatter(PaddingInfo padinfo)
        : FlagFormatter(padinfo)
    {
    }

    //-------------------------------------------------------------------------
    void ColorStopFormatter::format(const details::LogMsg& msg, const tm& /*unused*/, rsl::big_stack_string& dest)
    {
      msg.m_color_range_end = dest.size();
    }

    ///////////////////////////////////////////////////////////////////////
    // full formatter
    ///////////////////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    FullFormatter::FullFormatter(PaddingInfo padinfo)
        : FlagFormatter(padinfo)
    {
    }

    //-------------------------------------------------------------------------
    void FullFormatter::format(const details::LogMsg& msg, const tm& tmTime, rsl::big_stack_string& dest)
    {
      // cache the date/time part for the next second.
      auto duration = msg.time().time_since_epoch();
      auto secs     = rsl::chrono::duration_cast<rsl::chrono::seconds>(duration);

      if(m_cache_timestamp != secs || m_cached_datetime.empty())
      {
        m_cached_datetime.clear();
        m_cached_datetime.push_back('[');
        fmt_helper::append_int(tmTime.tm_year + 1900, m_cached_datetime);
        m_cached_datetime.push_back('-');

        fmt_helper::pad2(tmTime.tm_mon + 1, m_cached_datetime);
        m_cached_datetime.push_back('-');

        fmt_helper::pad2(tmTime.tm_mday, m_cached_datetime);
        m_cached_datetime.push_back(' ');

        fmt_helper::pad2(tmTime.tm_hour, m_cached_datetime);
        m_cached_datetime.push_back(':');

        fmt_helper::pad2(tmTime.tm_min, m_cached_datetime);
        m_cached_datetime.push_back(':');

        fmt_helper::pad2(tmTime.tm_sec, m_cached_datetime);
        m_cached_datetime.push_back('.');

        m_cache_timestamp = secs;
      }

      dest.append(m_cached_datetime.data(), m_cached_datetime.size());

      auto millis = fmt_helper::time_fraction<rsl::chrono::milliseconds>(msg.time());
      fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
      dest.push_back(']');
      dest.push_back(' ');

      // append logger name if exists
      if(!msg.logger_name().empty())
      {
        dest.push_back('[');
        fmt_helper::append_string_view(msg.logger_name(), dest);
        dest.push_back(']');
        dest.push_back(' ');
      }

      dest.push_back('[');
      // wrap the level name with color
      msg.m_color_range_start = dest.size();
      fmt_helper::append_string_view(level::to_string_view(msg.level()), dest);
      msg.m_color_range_end = dest.size();
      dest.push_back(']');
      dest.push_back(' ');

      // add source location if present
      if(!msg.source().file_name().empty())
      {
        dest.push_back('[');
        const char* filename = details::ShortFileNameFormatter<details::NullScopedPadder>::basename(msg.source().file_name().data());
        fmt_helper::append_string_view(rsl::string_view(filename), dest);
        dest.push_back(':');
        fmt_helper::append_int(msg.source().line(), dest);
        dest.push_back(']');
        dest.push_back(' ');
      }
      fmt_helper::append_string_view(msg.payload(), dest);
    }
  } // namespace details
} // namespace rexlog