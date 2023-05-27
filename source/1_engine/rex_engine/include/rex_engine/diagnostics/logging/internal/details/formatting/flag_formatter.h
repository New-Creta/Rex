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
            DayNames() = delete;
            DayNames(const DayNames& other) = delete;
            DayNames(DayNames&& other) = delete;

            DayNames& operator=(const DayNames& other) = delete;
            DayNames& operator=(DayNames&& other) = delete;

            // Abbreviated weekday name
            static const rsl::array<const char*, 7> days;
            // Full weekday name
            static const rsl::array<const char*, 7> full_days;
        };

        struct MonthNames
        {
            MonthNames() = delete;
            MonthNames(const MonthNames& other) = delete;
            MonthNames(MonthNames&& other) = delete;
            
            MonthNames& operator=(const MonthNames& other) = delete;
            MonthNames& operator=(MonthNames&& other) = delete;

            // Abbreviated month
            static const rsl::array<const char*, 12> months;
            // Full month name
            static const rsl::array<const char*, 12> full_months;
        };

        //-------------------------------------------------------------------------
        class FlagFormatter
        {
        public:
            FlagFormatter();
            explicit FlagFormatter(PaddingInfo padinfo);
            virtual ~FlagFormatter();

            virtual void format(const details::LogMsg& msg, const tm& tmTime, memory_buf_t& dest) = 0;

        protected:
            void set_padding_info(const details::PaddingInfo& padding);
            const PaddingInfo& padding_info() const;

        private:
            PaddingInfo m_padinfo;
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(msg.logger_name.size(), padding_info(), dest);
                fmt_helper::append_string_view(msg.logger_name, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(level_name.size(), padding_info(), dest);
                fmt_helper::append_string_view(level_name, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                const string_view_t level_name{ level::to_short_c_str(msg.level) };
                ScopedPadder p(level_name.size(), padding_info(), dest);
                fmt_helper::append_string_view(level_name, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                const string_view_t field_value{ DayNames::days[static_cast<count_t>(tm_time.tm_wday)] };
                ScopedPadder p(field_value.size(), padding_info(), dest);
                fmt_helper::append_string_view(field_value, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                const string_view_t field_value{ DayNames::full_days[static_cast<count_t>(tm_time.tm_wday)] };
                ScopedPadder p(field_value.size(), padding_info(), dest);
                fmt_helper::append_string_view(field_value, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                const string_view_t field_value{ MonthNames::months[static_cast<count_t>(tm_time.tm_mon)] };
                ScopedPadder p(field_value.size(), padding_info(), dest);
                fmt_helper::append_string_view(field_value, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                const string_view_t field_value{ MonthNames::full_months[static_cast<count_t>(tm_time.tm_mon)] };
                ScopedPadder p(field_value.size(), padding_info(), dest);
                fmt_helper::append_string_view(field_value, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);

                fmt_helper::append_string_view(string_view_t(DayNames::days[static_cast<count_t>(tm_time.tm_wday)]), dest);
                dest.push_back(' ');
                fmt_helper::append_string_view(string_view_t(MonthNames::months[static_cast<count_t>(tm_time.tm_mon)]), dest);
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

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad2(tm_time.tm_year % 100, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);

                fmt_helper::pad2(tm_time.tm_mon + 1, dest);
                dest.push_back('/');
                fmt_helper::pad2(tm_time.tm_mday, dest);
                dest.push_back('/');
                fmt_helper::pad2(tm_time.tm_year % 100, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::append_int(tm_time.tm_year + 1900, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad2(tm_time.tm_mon + 1, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad2(tm_time.tm_mday, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad2(tm_time.tm_hour, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad2(to12h(tm_time), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad2(tm_time.tm_min, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad2(tm_time.tm_sec, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                auto millis = fmt_helper::time_fraction<rsl::chrono::milliseconds>(msg.time);
                const size_t field_size = 3;
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad6(static_cast<size_t>(micros.count()), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                auto ns = fmt_helper::time_fraction<rsl::chrono::nanoseconds>(msg.time);
                const size_t field_size = 9;
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::pad9(static_cast<size_t>(ns.count()), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                auto duration = msg.time.time_since_epoch();
                auto seconds = rsl::chrono::duration_cast<rsl::chrono::seconds>(duration).count();
                fmt_helper::append_int(seconds, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::append_string_view(string_view_t(ampm(tm_time)), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);

                fmt_helper::pad2(to12h(tm_time), dest);
                dest.push_back(':');
                fmt_helper::pad2(tm_time.tm_min, dest);
                dest.push_back(':');
                fmt_helper::pad2(tm_time.tm_sec, dest);
                dest.push_back(' ');
                fmt_helper::append_string_view(string_view_t(ampm(tm_time)), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);

                fmt_helper::pad2(tm_time.tm_hour, dest);
                dest.push_back(':');
                fmt_helper::pad2(tm_time.tm_min, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);

                fmt_helper::pad2(tm_time.tm_hour, dest);
                dest.push_back(':');
                fmt_helper::pad2(tm_time.tm_min, dest);
                dest.push_back(':');
                fmt_helper::pad2(tm_time.tm_sec, dest);
            }
        };

        //-------------------------------------------------------------------------
        // ISO 8601 offset from UTC in timezone (+-HH:MM)
        template <typename ScopedPadder>
        class z_formatter final : public FlagFormatter
        {
        public:
            explicit z_formatter(PaddingInfo padinfo)
                : FlagFormatter(padinfo)
            {
            }

            z_formatter() = default;
            z_formatter(const z_formatter&) = delete;
            ~z_formatter() override = default;
            z_formatter& operator=(const z_formatter&) = delete;

            void format(const details::LogMsg& msg, const tm& tm_time, memory_buf_t& dest) override
            {
                const size_t field_size = 6;
                ScopedPadder p(field_size, padding_info(), dest);

                auto total_minutes = get_cached_offset(msg, tm_time);
                const bool is_negative = total_minutes < 0;
                if (is_negative)
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
            log_clock::time_point last_update_{ rsl::chrono::seconds(0) };
            int offset_minutes_{ 0 };

            int get_cached_offset(const LogMsg& msg, const tm& tm_time)
            {
                // refresh every 10 seconds
                if (msg.time - last_update_ >= rsl::chrono::seconds(10))
                {
                    offset_minutes_ = os::utc_minutes_offset(tm_time);
                    last_update_ = msg.time;
                }
                return offset_minutes_;
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::append_int(msg.thread_id, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                const auto pid = static_cast<uint32_t>(details::os::pid());
                auto field_size = ScopedPadder::count_digits(pid);
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::append_int(pid, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                ScopedPadder p(msg.payload.size(), padding_info(), dest);
                fmt_helper::append_string_view(msg.payload, dest);
            }
        };

        //-------------------------------------------------------------------------
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
                if (msg.source.file_name() != "")
                {
                    ScopedPadder p(0, padding_info(), dest);
                    return;
                }

                count_t text_size = 0;
                if (padding_info().enabled)
                {
                    // calc text size for padding based on "filename:line"
                    text_size = msg.source.file_name().length() + ScopedPadder::count_digits(msg.source.line()) + 1;
                }
                else
                {
                    text_size = 0;
                }

                ScopedPadder p(text_size, padding_info(), dest);
                fmt_helper::append_string_view(string_view_t(msg.source.file_name()), dest);
                dest.push_back(':');
                fmt_helper::append_int(msg.source.line(), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                if (msg.source.file_name() != "")
                {
                    ScopedPadder p(0, padding_info(), dest);
                    return;
                }
                const count_t text_size = padding_info().enabled ? msg.source.file_name().length() : 0;
                ScopedPadder p(text_size, padding_info(), dest);
                fmt_helper::append_string_view(string_view_t(msg.source.file_name()), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                if (sizeof(os::folder_seps) == 2)
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
                if (msg.source.file_name() != "")
                {
                    ScopedPadder p(0, padding_info(), dest);
                    return;
                }
                const auto filename = basename(msg.source.file_name().data()); // NOLINT(readability-qualified-auto)
                const count_t text_size = padding_info().enabled ? rsl::char_traits<char>::length(filename) : 0;
                ScopedPadder p(text_size, padding_info(), dest);
                fmt_helper::append_string_view(rsl::string_view(filename), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                if (msg.source.file_name() != "")
                {
                    ScopedPadder p(0, padding_info(), dest);
                    return;
                }

                auto field_size = ScopedPadder::count_digits(msg.source.line());
                ScopedPadder p(field_size, padding_info(), dest);
                fmt_helper::append_int(msg.source.line(), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                if (msg.source.file_name() != "")
                {
                    ScopedPadder p(0, padding_info(), dest);
                    return;
                }
                const count_t text_size = padding_info().enabled ? msg.source.function_name().length() : 0;
                ScopedPadder p(text_size, padding_info(), dest);
                fmt_helper::append_string_view(string_view_t(msg.source.function_name()), dest);
            }
        };

        //-------------------------------------------------------------------------
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
                auto delta = (rsl::max)(msg.time - last_message_time_, log_clock::duration::zero());
                auto delta_units = rsl::chrono::duration_cast<DurationUnits>(delta);
                last_message_time_ = msg.time;
                auto delta_count = static_cast<size_t>(delta_units.count());
                auto n_digits = static_cast<size_t>(ScopedPadder::count_digits(delta_count));
                ScopedPadder p(n_digits, padding_info(), dest);
                fmt_helper::append_int(delta_count, dest);
            }

        private:
            log_clock::time_point last_message_time_;
        };

        //-------------------------------------------------------------------------
        class ch_formatter final : public FlagFormatter
        {
        public:
            explicit ch_formatter(char ch);

            void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, memory_buf_t& dest) override;

        private:
            char ch_;
        };

        //-------------------------------------------------------------------------
        // aggregate user chars to display as is
        class aggregate_formatter final : public FlagFormatter
        {
        public:
            aggregate_formatter();

            void add_ch(char ch);
            void format(const details::LogMsg& /*unused*/, const tm& /*unused*/, memory_buf_t& dest) override;

        private:
            rex::DebugString str_;
        };

        //-------------------------------------------------------------------------
        // mark the color range. expect it to be in the form of "%^colored text%$"
        class color_start_formatter final : public FlagFormatter
        {
        public:
            explicit color_start_formatter(PaddingInfo padinfo);

            void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override;
        };

        //-------------------------------------------------------------------------
        class color_stop_formatter final : public FlagFormatter
        {
        public:
            explicit color_stop_formatter(PaddingInfo padinfo);

            void format(const details::LogMsg& msg, const tm& /*unused*/, memory_buf_t& dest) override;
        };

        //-------------------------------------------------------------------------
        // Full info formatter
        // pattern: [%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%s:%#] %v
        class full_formatter final : public FlagFormatter
        {
        public:
            explicit full_formatter(PaddingInfo padinfo);

            void format(const details::LogMsg& msg, const tm& tm_time, memory_buf_t& dest) override;

        private:
            rsl::chrono::seconds cache_timestamp_{ 0 };
            memory_buf_t cached_datetime_;
        };
    }
}