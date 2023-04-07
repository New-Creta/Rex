

#pragma once

// Loggers Registry of unique name->Logger pointer
// An attempt to create a Logger with an already existing name will result with rexlog_ex exception.
// If user requests a non existing Logger, nullptr will be returned
// This class is thread safe

#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/periodic_worker.h"

namespace rexlog
{
  class Logger;

  namespace details
  {
    class ThreadPool;

    class REXLOG_API Registry
    {
    public:
      using log_levels                     = rex::DebugHashTable<rex::DebugString, level::LevelEnum>;
      Registry(const Registry&)            = delete;
      Registry& operator=(const Registry&) = delete;

      void register_logger(rsl::shared_ptr<Logger> newLogger);
      void initialize_logger(rsl::shared_ptr<Logger> newLogger);
      rsl::shared_ptr<Logger> get(const rex::DebugString& loggerName);
      rsl::shared_ptr<Logger> default_logger();

      // Return raw ptr to the default Logger.
      // To be used directly by the rexlog default api (e.g. rexlog::info)
      // This make the default API faster, but cannot be used concurrently with set_default_logger().
      // e.g do not call set_default_logger() from one thread while calling rexlog::info() from another.
      Logger* get_default_raw();

      // set default Logger.
      // default Logger is stored in default_logger_ (for faster retrieval) and in the loggers_ map.
      void set_default_logger(rsl::shared_ptr<Logger> newDefaultLogger);

      void set_tp(rsl::shared_ptr<ThreadPool> tp);

      rsl::shared_ptr<ThreadPool> get_tp();

      // Set global formatter. Each sink in each Logger will get a clone of this object
      void set_formatter(rsl::unique_ptr<formatter> formatter);

      void enable_backtrace(size_t nMessages);

      void disable_backtrace();

      void set_level(level::LevelEnum logLevel);

      void flush_on(level::LevelEnum logLevel);

      template <typename Rep, typename Period>
      void flush_every(rsl::chrono::duration<Rep, Period> interval)
      {
        rsl::unique_lock<rsl::mutex> lock(flusher_mutex_);
        auto clbk         = [this]() { this->flush_all(); };
        periodic_flusher_ = details::make_unique<periodic_worker>(clbk, interval);
      }

      void set_error_handler(err_handler handler);

      void apply_all(const rsl::function<void(const rsl::shared_ptr<Logger>)>& fun);

      void flush_all();

      void drop(const rex::DebugString& loggerName);

      void drop_all();

      // clean all resources and threads started by the Registry
      void shutdown();

      rsl::recursive_mutex& tp_mutex();

      void set_automatic_registration(bool automaticRegistration);

      // set levels for all existing/future loggers. global_level can be null if should not set.
      void set_levels(log_levels levels, const level::LevelEnum* globalLevel);

      static Registry& instance();

      void apply_logger_env_levels(rsl::shared_ptr<Logger> newLogger);

    private:
      Registry();
      ~Registry();

      void throw_if_exists_impl(const rex::DebugString& loggerName);
      void register_logger_impl(rsl::shared_ptr<Logger> newLogger);
      bool set_level_from_cfg_impl(Logger* logger);
      rsl::mutex m_logger_map_mutex, m_flusher_mutex;
      rsl::recursive_mutex m_tp_mutex;
      rex::DebugHashTable<rex::DebugString, rsl::shared_ptr<Logger>> m_loggers;
      log_levels m_log_levels;
      rsl::unique_ptr<formatter> m_formatter;
      rexlog::level::LevelEnum m_global_log_level = level::Info;
      level::LevelEnum m_flush_level              = level::Off;
      err_handler m_err_handler;
      rsl::shared_ptr<ThreadPool> m_tp;
      rsl::unique_ptr<PeriodicWorker> m_periodic_flusher;
      rsl::shared_ptr<Logger> m_default_logger;
      bool m_automatic_registration = true;
      size_t m_backtrace_n_messages = 0;
    };

  } // namespace details
} // namespace rexlog
