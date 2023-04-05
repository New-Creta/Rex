

#pragma once

// Loggers Registry of unique name->logger pointer
// An attempt to create a logger with an already existing name will result with rexlog_ex exception.
// If user requests a non existing logger, nullptr will be returned
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
  class logger;

  namespace details
  {
    class thread_pool;

    class REXLOG_API Registry
    {
    public:
      using log_levels                     = rex::DebugHashTable<rex::DebugString, level::level_enum>;
      Registry(const Registry&)            = delete;
      Registry& operator=(const Registry&) = delete;

      void register_logger(rsl::shared_ptr<logger> new_logger);
      void initialize_logger(rsl::shared_ptr<logger> new_logger);
      rsl::shared_ptr<logger> get(const rex::DebugString& logger_name);
      rsl::shared_ptr<logger> default_logger();

      // Return raw ptr to the default logger.
      // To be used directly by the rexlog default api (e.g. rexlog::info)
      // This make the default API faster, but cannot be used concurrently with set_default_logger().
      // e.g do not call set_default_logger() from one thread while calling rexlog::info() from another.
      logger* get_default_raw();

      // set default logger.
      // default logger is stored in default_logger_ (for faster retrieval) and in the loggers_ map.
      void set_default_logger(rsl::shared_ptr<logger> new_default_logger);

      void set_tp(rsl::shared_ptr<thread_pool> tp);

      rsl::shared_ptr<thread_pool> get_tp();

      // Set global formatter. Each sink in each logger will get a clone of this object
      void set_formatter(rsl::unique_ptr<formatter> formatter);

      void enable_backtrace(size_t n_messages);

      void disable_backtrace();

      void set_level(level::level_enum log_level);

      void flush_on(level::level_enum log_level);

      template <typename Rep, typename Period>
      void flush_every(rsl::chrono::duration<Rep, Period> interval)
      {
        rsl::unique_lock<rsl::mutex> lock(flusher_mutex_);
        auto clbk         = [this]() { this->flush_all(); };
        periodic_flusher_ = details::make_unique<periodic_worker>(clbk, interval);
      }

      void set_error_handler(err_handler handler);

      void apply_all(const rsl::function<void(const rsl::shared_ptr<logger>)>& fun);

      void flush_all();

      void drop(const rex::DebugString& logger_name);

      void drop_all();

      // clean all resources and threads started by the Registry
      void shutdown();

      rsl::recursive_mutex& tp_mutex();

      void set_automatic_registration(bool automatic_registration);

      // set levels for all existing/future loggers. global_level can be null if should not set.
      void set_levels(log_levels levels, level::level_enum* global_level);

      static Registry& instance();

      void apply_logger_env_levels(rsl::shared_ptr<logger> new_logger);

    private:
      Registry();
      ~Registry();

      void throw_if_exists_impl(const rex::DebugString& logger_name);
      void register_logger_impl(rsl::shared_ptr<logger> new_logger);
      bool set_level_from_cfg_impl(logger* logger);
      rsl::mutex logger_map_mutex_, flusher_mutex_;
      rsl::recursive_mutex tp_mutex_;
      rex::DebugHashTable<rex::DebugString, rsl::shared_ptr<logger>> loggers_;
      log_levels log_levels_;
      rsl::unique_ptr<formatter> m_formatter;
      rexlog::level::level_enum global_log_level_ = level::info;
      level::level_enum flush_level_              = level::off;
      err_handler err_handler_;
      rsl::shared_ptr<thread_pool> tp_;
      rsl::unique_ptr<periodic_worker> periodic_flusher_;
      rsl::shared_ptr<logger> default_logger_;
      bool automatic_registration_ = true;
      size_t backtrace_n_messages_ = 0;
    };

  } // namespace details
} // namespace rexlog
