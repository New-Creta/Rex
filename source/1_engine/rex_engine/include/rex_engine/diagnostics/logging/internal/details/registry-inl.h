

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/periodic_worker.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/types.h"

#ifndef REXLOG_DISABLE_DEFAULT_LOGGER
  // support for the default stdout color Logger
  #ifdef _WIN32
    #include "rex_engine/diagnostics/logging/internal/sinks/wincolor_sink.h"
  #else
    #include "rex_engine/diagnostics/logging/internal/sinks/ansicolor_sink.h"
  #endif
#endif // REXLOG_DISABLE_DEFAULT_LOGGER

#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE Registry::Registry()
        : m_formatter(new PatternFormatter())
    {
#ifndef REXLOG_DISABLE_DEFAULT_LOGGER
      // create default Logger (ansicolor_stdout_sink_mt or wincolor_stdout_sink_mt in windows).
  #ifdef _WIN32
      auto color_sink = rsl::allocate_shared<sinks::wincolor_stdout_sink_mt>(rex::global_debug_allocator());
  #else
      auto color_sink = rsl::allocate_shared<sinks::ansicolor_stdout_sink_mt>(rex::global_debug_allocator());
  #endif

      const char* default_logger_name                  = "";
      m_default_logger                                 = rsl::allocate_shared<rexlog::Logger>(rex::global_debug_allocator(), rex::DebugString(default_logger_name), rsl::move(color_sink));
      m_loggers[rex::DebugString(default_logger_name)] = m_default_logger;

#endif // REXLOG_DISABLE_DEFAULT_LOGGER
    }

    REXLOG_INLINE Registry::~Registry() = default;

    REXLOG_INLINE void Registry::register_logger(rsl::shared_ptr<Logger> newLogger)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      register_logger_impl(rsl::move(newLogger));
    }

    REXLOG_INLINE void Registry::initialize_logger(rsl::shared_ptr<Logger> newLogger)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      newLogger->set_formatter(m_formatter->clone());

      if(m_err_handler)
      {
        newLogger->set_error_handler(m_err_handler);
      }

      // set new level according to previously configured level or default level
      auto it        = m_log_levels.find(newLogger->name());
      auto new_level = it != m_log_levels.end() ? it->value : m_global_log_level;
      newLogger->set_level(new_level);

      newLogger->flush_on(m_flush_level);

      if(m_backtrace_n_messages > 0)
      {
        newLogger->enable_backtrace(m_backtrace_n_messages);
      }

      if(m_automatic_registration)
      {
        register_logger_impl(newLogger);
      }
    }

    REXLOG_INLINE rsl::shared_ptr<Logger> Registry::get(const rex::DebugString& loggerName)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      auto found = m_loggers.find(loggerName);
      return found == m_loggers.end() ? nullptr : found->value;
    }

    REXLOG_INLINE rsl::shared_ptr<Logger> Registry::default_logger()
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      return m_default_logger;
    }

    // Return raw ptr to the default Logger.
    // To be used directly by the rexlog default api (e.g. rexlog::info)
    // This make the default API faster, but cannot be used concurrently with set_default_logger().
    // e.g do not call set_default_logger() from one thread while calling rexlog::info() from another.
    REXLOG_INLINE Logger* Registry::get_default_raw()
    {
      return m_default_logger.get();
    }

    // set default Logger.
    // default Logger is stored in default_logger_ (for faster retrieval) and in the loggers_ map.
    REXLOG_INLINE void Registry::set_default_logger(rsl::shared_ptr<Logger> newDefaultLogger)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      // remove previous default Logger from the map
      if(m_default_logger != nullptr)
      {
        m_loggers.erase(m_default_logger->name());
      }
      if(newDefaultLogger != nullptr)
      {
        m_loggers[newDefaultLogger->name()] = newDefaultLogger;
      }
      m_default_logger = rsl::move(newDefaultLogger);
    }

    REXLOG_INLINE void Registry::set_tp(rsl::shared_ptr<ThreadPool> tp)
    {
      const rsl::unique_lock<rsl::recursive_mutex> lock(m_tp_mutex);
      m_tp = rsl::move(tp);
    }

    REXLOG_INLINE rsl::shared_ptr<ThreadPool> Registry::get_tp()
    {
      const rsl::unique_lock<rsl::recursive_mutex> lock(m_tp_mutex);
      return m_tp;
    }

    // Set global formatter. Each sink in each Logger will get a clone of this object
    REXLOG_INLINE void Registry::set_formatter(rsl::unique_ptr<formatter> formatter)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      m_formatter = rsl::move(formatter);
      for(auto& l: m_loggers)
      {
        l.value->set_formatter(m_formatter->clone());
      }
    }

    REXLOG_INLINE void Registry::enable_backtrace(size_t nMessages)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      m_backtrace_n_messages = nMessages;

      for(auto& l: m_loggers)
      {
        l.value->enable_backtrace(nMessages);
      }
    }

    REXLOG_INLINE void Registry::disable_backtrace()
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      m_backtrace_n_messages = 0;
      for(auto& l: m_loggers)
      {
        l.value->disable_backtrace();
      }
    }

    REXLOG_INLINE void Registry::set_level(level::LevelEnum logLevel)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      for(auto& l: m_loggers)
      {
        l.value->set_level(logLevel);
      }
      m_global_log_level = logLevel;
    }

    REXLOG_INLINE void Registry::flush_on(level::LevelEnum logLevel)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      for(auto& l: m_loggers)
      {
        l.value->flush_on(logLevel);
      }
      m_flush_level = logLevel;
    }

    REXLOG_INLINE void Registry::set_error_handler(err_handler handler)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      for(auto& l: m_loggers)
      {
        l.value->set_error_handler(handler);
      }
      m_err_handler = rsl::move(handler);
    }

    REXLOG_INLINE void Registry::apply_all(const rsl::function<void(const rsl::shared_ptr<Logger>)>& fun)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      for(auto& l: m_loggers)
      {
        fun(l.value);
      }
    }

    REXLOG_INLINE void Registry::flush_all()
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      for(auto& l: m_loggers)
      {
        l.value->flush();
      }
    }

    REXLOG_INLINE void Registry::drop(const rex::DebugString& loggerName)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      m_loggers.erase(loggerName);
      if(m_default_logger && m_default_logger->name() == loggerName)
      {
        m_default_logger.reset();
      }
    }

    REXLOG_INLINE void Registry::drop_all()
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      m_loggers.clear();
      m_default_logger.reset();
    }

    // clean all resources and threads started by the Registry
    REXLOG_INLINE void Registry::shutdown()
    {
      {
        const rsl::unique_lock<rsl::mutex> lock(m_flusher_mutex);
        m_periodic_flusher.reset();
      }

      drop_all();

      {
        const rsl::unique_lock<rsl::recursive_mutex> lock(m_tp_mutex);
        m_tp.reset();
      }
    }

    REXLOG_INLINE rsl::recursive_mutex& Registry::tp_mutex()
    {
      return m_tp_mutex;
    }

    REXLOG_INLINE void Registry::set_automatic_registration(bool automaticRegistration)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      m_automatic_registration = automaticRegistration;
    }

    REXLOG_INLINE void Registry::set_levels(log_levels levels, const level::LevelEnum* globalLevel)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      m_log_levels                = rsl::move(levels);
      auto global_level_requested = globalLevel != nullptr;
      m_global_log_level          = global_level_requested ? *globalLevel : m_global_log_level;

      for(auto& logger: m_loggers)
      {
        auto logger_entry = m_log_levels.find(logger.key);
        if(logger_entry != m_log_levels.end())
        {
          logger.value->set_level(logger_entry->value);
        }
        else if(global_level_requested)
        {
          logger.value->set_level(*globalLevel);
        }
      }
    }

    REXLOG_INLINE Registry& Registry::instance()
    {
      static Registry s_instance;
      return s_instance;
    }

    REXLOG_INLINE void Registry::apply_logger_env_levels(rsl::shared_ptr<Logger> newLogger)
    {
      const rsl::unique_lock<rsl::mutex> lock(m_logger_map_mutex);
      auto it        = m_log_levels.find(newLogger->name());
      auto new_level = it != m_log_levels.end() ? it->value : m_global_log_level;
      newLogger->set_level(new_level);
    }

    REXLOG_INLINE void Registry::throw_if_exists_impl(const rex::DebugString& loggerName)
    {
      if(m_loggers.find(loggerName) != m_loggers.end())
      {
        throw_rexlog_ex("Logger with name '" + loggerName + "' already exists");
      }
    }

    REXLOG_INLINE void Registry::register_logger_impl(rsl::shared_ptr<Logger> newLogger)
    {
      auto logger_name = rex::DebugString(newLogger->name());
      throw_if_exists_impl(logger_name);
      m_loggers[logger_name] = rsl::move(newLogger);
    }

  } // namespace details
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)
