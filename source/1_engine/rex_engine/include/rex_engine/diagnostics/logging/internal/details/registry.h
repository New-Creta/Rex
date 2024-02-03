

#pragma once

// Loggers Registry of unique name->Logger pointer
// An attempt to create a Logger with an already existing name will result with rexlog_ex exception.
// If user requests a non existing Logger, nullptr will be returned
// This class is thread safe

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/engine/debug_types.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace log
  {
    class Logger;

    using LoggerObjectPtr    = rsl::shared_ptr<Logger>;
    using LoggerObjectPtrMap = rex::DebugHashTable<rex::DebugString, LoggerObjectPtr>;

    namespace details
    {
      class ThreadPool;

      class Registry
      {
      public:
        using LogLevels = rsl::unordered_map<rsl::string_view, level::LevelEnum>;

        static Registry& instance();

        Registry(const Registry&)            = delete;
        Registry& operator=(const Registry&) = delete;

      public:
        void register_logger(LoggerObjectPtr newLogger);
        void initialize_logger(LoggerObjectPtr newLogger);

        LoggerObjectPtr get(rsl::string_view loggerName);
        level::LevelEnum get_global_level() const;

        void set_thread_pool(rsl::shared_ptr<ThreadPool> tp);
        void set_formatter(PatternFormatter&& formatter);
        void set_level(level::LevelEnum logLevel);
        void set_levels(LogLevels levels, const level::LevelEnum* globalLevel);

        void flush_on(level::LevelEnum logLevel);
        void flush_all();

        void shutdown();

        rsl::shared_ptr<ThreadPool> thread_pool();
        rsl::recursive_mutex& thread_pool_mutex();

      private:
        Registry();
        ~Registry();

        void register_logger_impl(LoggerObjectPtr newLogger);

        LoggerObjectPtrMap m_loggers;
        LogLevels m_log_levels;

        rsl::mutex m_logger_map_mutex;
        rsl::mutex m_flusher_mutex;
        rsl::recursive_mutex m_tp_mutex;

        PatternFormatter m_formatter;
        rex::log::level::LevelEnum m_global_log_level;
        level::LevelEnum m_flush_level;
        rsl::shared_ptr<ThreadPool> m_tp;
      };

    } // namespace details
  }   // namespace log
} // namespace rex
