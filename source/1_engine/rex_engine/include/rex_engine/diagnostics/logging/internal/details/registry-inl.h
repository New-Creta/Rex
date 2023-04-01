

#pragma once

#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/details/periodic_worker.h>
#include <rex_engine/diagnostics/logging/internal/details/registry.h>
#include <rex_engine/diagnostics/logging/internal/logger.h>
#include <rex_engine/diagnostics/logging/internal/pattern_formatter.h>

#ifndef REXLOG_DISABLE_DEFAULT_LOGGER
  // support for the default stdout color logger
  #ifdef _WIN32
    #include <rex_engine/diagnostics/logging/internal/sinks/wincolor_sink.h>
  #else
    #include <rex_engine/diagnostics/logging/internal/sinks/ansicolor_sink.h>
  #endif
#endif // REXLOG_DISABLE_DEFAULT_LOGGER

#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE registry::registry()
        : formatter_(new pattern_formatter())
    {
#ifndef REXLOG_DISABLE_DEFAULT_LOGGER
      // create default logger (ansicolor_stdout_sink_mt or wincolor_stdout_sink_mt in windows).
  #ifdef _WIN32
      auto color_sink = rsl::make_shared<sinks::wincolor_stdout_sink_mt>();
  #else
      auto color_sink = rsl::make_shared<sinks::ansicolor_stdout_sink_mt>();
  #endif

      const char* default_logger_name            = "";
      default_logger_                            = rsl::make_shared<rexlog::logger>(rex::DebugString(default_logger_name), rsl::move(color_sink));
      loggers_[rex::DebugString(default_logger_name)] = default_logger_;

#endif // REXLOG_DISABLE_DEFAULT_LOGGER
    }

    REXLOG_INLINE registry::~registry() = default;

    REXLOG_INLINE void registry::register_logger(rsl::shared_ptr<logger> new_logger)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      register_logger_(rsl::move(new_logger));
    }

    REXLOG_INLINE void registry::initialize_logger(rsl::shared_ptr<logger> new_logger)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      new_logger->set_formatter(formatter_->clone());

      if(err_handler_)
      {
        new_logger->set_error_handler(err_handler_);
      }

      // set new level according to previously configured level or default level
      auto it        = log_levels_.find(new_logger->name());
      auto new_level = it != log_levels_.end() ? it->value : global_log_level_;
      new_logger->set_level(new_level);

      new_logger->flush_on(flush_level_);

      if(backtrace_n_messages_ > 0)
      {
        new_logger->enable_backtrace(backtrace_n_messages_);
      }

      if(automatic_registration_)
      {
        register_logger_(rsl::move(new_logger));
      }
    }

    REXLOG_INLINE rsl::shared_ptr<logger> registry::get(const rex::DebugString& logger_name)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      auto found = loggers_.find(logger_name);
      return found == loggers_.end() ? nullptr : found->value;
    }

    REXLOG_INLINE rsl::shared_ptr<logger> registry::default_logger()
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      return default_logger_;
    }

    // Return raw ptr to the default logger.
    // To be used directly by the rexlog default api (e.g. rexlog::info)
    // This make the default API faster, but cannot be used concurrently with set_default_logger().
    // e.g do not call set_default_logger() from one thread while calling rexlog::info() from another.
    REXLOG_INLINE logger* registry::get_default_raw()
    {
      return default_logger_.get();
    }

    // set default logger.
    // default logger is stored in default_logger_ (for faster retrieval) and in the loggers_ map.
    REXLOG_INLINE void registry::set_default_logger(rsl::shared_ptr<logger> new_default_logger)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      // remove previous default logger from the map
      if(default_logger_ != nullptr)
      {
        loggers_.erase(default_logger_->name());
      }
      if(new_default_logger != nullptr)
      {
        loggers_[new_default_logger->name()] = new_default_logger;
      }
      default_logger_ = rsl::move(new_default_logger);
    }

    REXLOG_INLINE void registry::set_tp(rsl::shared_ptr<thread_pool> tp)
    {
      rsl::unique_lock<rsl::recursive_mutex> lock(tp_mutex_);
      tp_ = rsl::move(tp);
    }

    REXLOG_INLINE rsl::shared_ptr<thread_pool> registry::get_tp()
    {
      rsl::unique_lock<rsl::recursive_mutex> lock(tp_mutex_);
      return tp_;
    }

    // Set global formatter. Each sink in each logger will get a clone of this object
    REXLOG_INLINE void registry::set_formatter(rsl::unique_ptr<formatter> formatter)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      formatter_ = rsl::move(formatter);
      for(auto& l: loggers_)
      {
        l.value->set_formatter(formatter_->clone());
      }
    }

    REXLOG_INLINE void registry::enable_backtrace(size_t n_messages)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      backtrace_n_messages_ = n_messages;

      for(auto& l: loggers_)
      {
        l.value->enable_backtrace(n_messages);
      }
    }

    REXLOG_INLINE void registry::disable_backtrace()
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      backtrace_n_messages_ = 0;
      for(auto& l: loggers_)
      {
        l.value->disable_backtrace();
      }
    }

    REXLOG_INLINE void registry::set_level(level::level_enum log_level)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      for(auto& l: loggers_)
      {
        l.value->set_level(log_level);
      }
      global_log_level_ = log_level;
    }

    REXLOG_INLINE void registry::flush_on(level::level_enum log_level)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      for(auto& l: loggers_)
      {
        l.value->flush_on(log_level);
      }
      flush_level_ = log_level;
    }

    REXLOG_INLINE void registry::set_error_handler(err_handler handler)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      for(auto& l: loggers_)
      {
        l.value->set_error_handler(handler);
      }
      err_handler_ = rsl::move(handler);
    }

    REXLOG_INLINE void registry::apply_all(const rsl::function<void(const rsl::shared_ptr<logger>)>& fun)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      for(auto& l: loggers_)
      {
        fun(l.value);
      }
    }

    REXLOG_INLINE void registry::flush_all()
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      for(auto& l: loggers_)
      {
        l.value->flush();
      }
    }

    REXLOG_INLINE void registry::drop(const rex::DebugString& logger_name)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      loggers_.erase(logger_name);
      if(default_logger_ && default_logger_->name() == logger_name)
      {
        default_logger_.reset();
      }
    }

    REXLOG_INLINE void registry::drop_all()
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      loggers_.clear();
      default_logger_.reset();
    }

    // clean all resources and threads started by the registry
    REXLOG_INLINE void registry::shutdown()
    {
      {
        rsl::unique_lock<rsl::mutex> lock(flusher_mutex_);
        periodic_flusher_.reset();
      }

      drop_all();

      {
        rsl::unique_lock<rsl::recursive_mutex> lock(tp_mutex_);
        tp_.reset();
      }
    }

    REXLOG_INLINE rsl::recursive_mutex& registry::tp_mutex()
    {
      return tp_mutex_;
    }

    REXLOG_INLINE void registry::set_automatic_registration(bool automatic_registration)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      automatic_registration_ = automatic_registration;
    }

    REXLOG_INLINE void registry::set_levels(log_levels levels, level::level_enum* global_level)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      log_levels_                 = rsl::move(levels);
      auto global_level_requested = global_level != nullptr;
      global_log_level_           = global_level_requested ? *global_level : global_log_level_;

      for(auto& logger: loggers_)
      {
        auto logger_entry = log_levels_.find(logger.key);
        if(logger_entry != log_levels_.end())
        {
          logger.value->set_level(logger_entry->value);
        }
        else if(global_level_requested)
        {
          logger.value->set_level(*global_level);
        }
      }
    }

    REXLOG_INLINE registry& registry::instance()
    {
      static registry s_instance;
      return s_instance;
    }

    REXLOG_INLINE void registry::apply_logger_env_levels(rsl::shared_ptr<logger> new_logger)
    {
      rsl::unique_lock<rsl::mutex> lock(logger_map_mutex_);
      auto it        = log_levels_.find(new_logger->name());
      auto new_level = it != log_levels_.end() ? it->value : global_log_level_;
      new_logger->set_level(new_level);
    }

    REXLOG_INLINE void registry::throw_if_exists_(const rex::DebugString& logger_name)
    {
      if(loggers_.find(logger_name) != loggers_.end())
      {
        throw_rexlog_ex("logger with name '" + logger_name + "' already exists");
      }
    }

    REXLOG_INLINE void registry::register_logger_(rsl::shared_ptr<logger> new_logger)
    {
      auto logger_name = rex::DebugString(new_logger->name());
      throw_if_exists_(logger_name);
      loggers_[logger_name] = rsl::move(new_logger);
    }

  } // namespace details
} // namespace rexlog
