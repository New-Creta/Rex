#include "rex_engine/diagnostics/logging/internal/details/formatting/flag_formatter.h"
#include "rex_engine/diagnostics/logging/internal/details/formatting/padder.h"
#include "rex_engine/diagnostics/logging/internal/details/fmt_helper.h"

namespace rexlog
{
    namespace details
    {
        ///////////////////////////////////////////////////////////////////////
        // Time information
        ///////////////////////////////////////////////////////////////////////
        const rsl::array<const char*, 7>    DayNames::days = { {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"} };
        const rsl::array<const char*, 7>    DayNames::full_days = { {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"} };
        const rsl::array<const char*, 12>   MonthNames::months = { {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"} };
        const rsl::array<const char*, 12>   MonthNames::full_months = { {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"} };


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
        FlagFormatter::FlagFormatter()
        {
            // Nothing to implement
        }
        FlagFormatter::FlagFormatter(PaddingInfo padinfo)
            : m_padinfo(padinfo)
        {
            // Nothing to implement
        }

        FlagFormatter::~FlagFormatter()
        {
            // Nothingto implement
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
        ch_formatter::ch_formatter(char ch)
            : ch_(ch)
        {
        }

        //-------------------------------------------------------------------------
        void ch_formatter::format(const details::LogMsg& /*unused*/, const tm& /*unused*/, memory_buf_t& dest)
        {
            dest.push_back(ch_);
        }

        ///////////////////////////////////////////////////////////////////////
        // aggregate formatter
        ///////////////////////////////////////////////////////////////////////

        //-------------------------------------------------------------------------
        // aggregate user chars to display as is
        aggregate_formatter::aggregate_formatter()
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void aggregate_formatter::add_ch(char ch)
        {
            str_ += ch;
        }

        //-------------------------------------------------------------------------
        void aggregate_formatter::format(const details::LogMsg& /*unused*/, const tm& /*unused*/, memory_buf_t& dest)
        {
            fmt_helper::append_string_view(str_, dest);
        }

        ///////////////////////////////////////////////////////////////////////
        // color start formatter
        ///////////////////////////////////////////////////////////////////////

        //-------------------------------------------------------------------------
        color_start_formatter::color_start_formatter(PaddingInfo padinfo)
            : FlagFormatter(padinfo)
        {
        }

        //-------------------------------------------------------------------------
        void color_start_formatter::format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest)
        {
            msg.color_range_start = dest.size();
        }

        ///////////////////////////////////////////////////////////////////////
        // color stop formatter
        ///////////////////////////////////////////////////////////////////////

        //-------------------------------------------------------------------------
        color_stop_formatter::color_stop_formatter(PaddingInfo padinfo)
            : FlagFormatter(padinfo)
        {
        }

        //-------------------------------------------------------------------------
        void color_stop_formatter::format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest)
        {
            msg.color_range_end = dest.size();
        }

        ///////////////////////////////////////////////////////////////////////
        // full formatter
        ///////////////////////////////////////////////////////////////////////

        //-------------------------------------------------------------------------
        full_formatter::full_formatter(PaddingInfo padinfo)
            : FlagFormatter(padinfo)
        {
        }

        //-------------------------------------------------------------------------
        void full_formatter::format(const details::LogMsg& msg, const tm& tm_time, memory_buf_t& dest)
        {
            using rsl::chrono::duration_cast;
            using rsl::chrono::milliseconds;
            using rsl::chrono::seconds;

            // cache the date/time part for the next second.
            auto duration = msg.time.time_since_epoch();
            auto secs = duration_cast<seconds>(duration);

            if (cache_timestamp_ != secs || cached_datetime_.empty())
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
            if (!msg.logger_name.empty())
            {
                dest.push_back('[');
                fmt_helper::append_string_view(string_view_t(msg.logger_name), dest);
                dest.push_back(']');
                dest.push_back(' ');
            }

            dest.push_back('[');
            // wrap the level name with color
            msg.color_range_start = dest.size();
            fmt_helper::append_string_view(level::to_string_view(msg.level), dest);
            msg.color_range_end = dest.size();
            dest.push_back(']');
            dest.push_back(' ');

            // add source location if present
            if (!msg.source.empty())
            {
                dest.push_back('[');
                const char* filename = details::short_filename_formatter<details::NullScopedPadder>::basename(msg.source.filename);
                fmt_helper::append_string_view(rsl::string_view(filename), dest);
                dest.push_back(':');
                fmt_helper::append_int(msg.source.line, dest);
                dest.push_back(']');
                dest.push_back(' ');
            }
            fmt_helper::append_string_view(string_view_t(msg.payload), dest);
        }
    }
}