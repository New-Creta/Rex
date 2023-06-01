

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_std/source_location.h"

#include <string>

namespace rexlog
{
    namespace details
    {
        class LogMsg
        {
        public:
            mutable card32              m_color_range_start;
            mutable card32              m_color_range_end;

            LogMsg();
            LogMsg(log_clock::time_point logTime, const rsl::source_location& loc, rsl::string_view loggerName, level::LevelEnum lvl, rsl::string_view msg);
            LogMsg(const rsl::source_location& loc, rsl::string_view loggerName, level::LevelEnum lvl, rsl::string_view msg);
            LogMsg(rsl::string_view loggerName, level::LevelEnum lvl, rsl::string_view msg);

            void                        set_logger_name(rsl::string_view loggerName);
            void                        set_level(level::LevelEnum level);
            void                        set_time(log_clock::time_point tp);
            void                        set_thread_id(size_t id);

            void                        set_color_range_start(card32 cs);
            void                        set_color_range_end(card32 ce);

            void                        set_source(const rsl::source_location& sourceLoc);
            void                        set_payload(rsl::string_view p);

            rsl::string_view            logger_name() const;
            level::LevelEnum            level() const;
            log_clock::time_point       time() const;
            size_t                      thread_id() const;

            card32                      color_range_start() const;
            card32                      color_range_end() const;

            const rsl::source_location& source() const;
            rsl::string_view            payload() const;

        private:
            rsl::string_view            m_logger_name;
            level::LevelEnum            m_level;
            log_clock::time_point       m_time;
            size_t                      m_thread_id;

            rsl::source_location        m_source;
            rsl::string_view            m_payload;
        };
    } // namespace details
} // namespace rexlog