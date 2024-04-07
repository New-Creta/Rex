#include "rex_engine/diagnostics/logging/internal/details/registry.h"

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/memory.h"
#include "rex_std/string.h"

#include <cstdio>

// IWYU pragma: no_include <built-in>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rex
{
  namespace log
  {
    namespace details
    {
      //-------------------------------------------------------------------------
      Registry::Registry()
          : m_global_log_level(level::LevelEnum::Info)
          , m_flush_level(level::LevelEnum::Off)
      {
      }
      //-------------------------------------------------------------------------
      Registry::~Registry() = default;

      //-------------------------------------------------------------------------
      void Registry::register_logger(LoggerObjectPtr newLogger)
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);

        register_logger_impl(rsl::move(newLogger));
      }

      //-------------------------------------------------------------------------
      void Registry::initialize_logger(LoggerObjectPtr newLogger)
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);

        // set new level according to previously configured level or default level
        auto it        = m_log_levels.find(newLogger->name());
        auto new_level = it != m_log_levels.end() ? it->value : m_global_log_level;

        newLogger->set_level(new_level);
        newLogger->flush_on(m_flush_level);

        register_logger_impl(newLogger);
      }

      //-------------------------------------------------------------------------
      LoggerObjectPtr Registry::get(rsl::string_view loggerName)
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);

        auto found = m_loggers.find(loggerName);
        return found == m_loggers.end() ? nullptr : found->value;
      }

      //-------------------------------------------------------------------------
      rex::log::level::LevelEnum Registry::get_global_level() const
      {
        return m_global_log_level;
      }

      //-------------------------------------------------------------------------
      void Registry::set_thread_pool(rsl::shared_ptr<ThreadPool> tp)
      {
        const rsl::unique_lock<rsl::recursive_mutex> lock(m_tp_mutex);
        m_tp = rsl::move(tp);
      }

      //-------------------------------------------------------------------------
      void Registry::set_formatter(PatternFormatter&& formatter)
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);

        m_formatter = rsl::move(formatter);
        for(auto& l: m_loggers)
        {
          l.value->set_formatter(m_formatter.clone());
        }
      }

      //-------------------------------------------------------------------------
      void Registry::set_level(level::LevelEnum logLevel)
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
        for(auto& l: m_loggers)
        {
          l.value->set_level(logLevel);
        }

        m_global_log_level = logLevel;
      }

      //-------------------------------------------------------------------------
      void Registry::set_levels(LogLevels levels, const level::LevelEnum* globalLevel)
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);

        m_log_levels       = rsl::move(levels);
        m_global_log_level = globalLevel != nullptr ? *globalLevel : m_global_log_level;

        for(auto& logger: m_loggers)
        {
          auto level_entry = m_log_levels.find(logger.key);
          if(level_entry != m_log_levels.end())
          {
            logger.value->set_level(level_entry->value);
          }
          else if(globalLevel != nullptr)
          {
            logger.value->set_level(*globalLevel);
          }
        }
      }

      //-------------------------------------------------------------------------
      void Registry::flush_on(level::LevelEnum logLevel)
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
        for(auto& l: m_loggers)
        {
          l.value->flush_on(logLevel);
        }
        m_flush_level = logLevel;
      }

      //-------------------------------------------------------------------------
      void Registry::flush_all()
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
        for(auto& l: m_loggers)
        {
          l.value->flush();
        }
      }

      //-------------------------------------------------------------------------
      void Registry::shutdown()
      {
        const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);

        m_loggers.clear();
        m_tp.reset();
      }

      //-------------------------------------------------------------------------
      rsl::shared_ptr<ThreadPool> Registry::thread_pool()
      {
        const rsl::unique_lock<rsl::recursive_mutex> lock(m_tp_mutex);
        return m_tp;
      }

      //-------------------------------------------------------------------------
      rsl::recursive_mutex& Registry::thread_pool_mutex()
      {
        return m_tp_mutex;
      }

      //-------------------------------------------------------------------------
      Registry& Registry::instance()
      {
        static Registry s_instance;
        return s_instance;
      }

      void Registry::register_logger_impl(LoggerObjectPtr newLogger)
      {
        auto logger_name = rex::DebugString(newLogger->name());

        if(m_loggers.find(logger_name) != m_loggers.end())
        {
          rex::DebugString err(rex::global_debug_allocator());
          err += "Logger with name '";
          err += logger_name;
          err += "' already exists";
          printf("%s", err.data());
        }

        m_loggers[logger_name] = rsl::move(newLogger);
      }

    } // namespace details
  }   // namespace log
} // namespace rex

// NOLINTEND(misc-definitions-in-headers)
