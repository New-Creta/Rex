

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
            mutable size_t          m_color_range_start;
            mutable size_t          m_color_range_end;

            LogMsg();
            LogMsg(log_clock::time_point logTime, const rsl::source_location& loc, string_view_t loggerName, level::LevelEnum lvl, string_view_t msg);
            LogMsg(const rsl::source_location& loc, string_view_t loggerName, level::LevelEnum lvl, string_view_t msg);
            LogMsg(string_view_t loggerName, level::LevelEnum lvl, string_view_t msg);

            void                    set_logger_name(string_view_t loggerName);
            void                    set_level(level::LevelEnum level);
            void                    set_time(log_clock::time_point tp);
            void                    set_thread_id(size_t id);

            void                    set_color_range_start(size_t cs);
            void                    set_color_range_end(size_t ce);

            void                    set_source(const rsl::source_location& sourceLoc);
            void                    set_payload(string_view_t p);

            string_view_t           logger_name() const;
            level::LevelEnum        level() const;
            log_clock::time_point   time() const;
            size_t                  thread_id() const;

            size_t                  color_range_start() const;
            size_t                  color_range_end() const;

            SourceLoc               source() const;
            string_view_t           payload() const;

        private:
            string_view_t           m_logger_name;
            level::LevelEnum        m_level;
            log_clock::time_point   m_time;
            size_t                  m_thread_id;

            rsl::source_location    m_source;
            string_view_t           m_payload;
        };
    } // namespace details
} // namespace rexlog