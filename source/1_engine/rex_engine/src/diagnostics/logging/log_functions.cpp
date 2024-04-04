#include "rex_engine/diagnostics/logging/log_functions.h"

#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/details/diag_thread_pool.h"
#include "rex_engine/diagnostics/logging/internal/logger_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/basic_file_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/dist_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_color_sinks.h"
#include "rex_engine/engine/debug_types.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/vector.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"

#if defined(REX_BUILD_DEBUG) || defined(REX_BUILD_DEBUG_OPT)
  #define REX_ENABLE_COLOR_SINK
#endif

namespace rex
{
  namespace log
  {
    namespace sinks
    {
      class AbstractSink;
    } // namespace sinks
  }   // namespace log
} // namespace rex

namespace rex
{
  using LogPattern  = rsl::string_view;
  using LogLevelMap = DebugHashTable<LogVerbosity, rex::log::level::LevelEnum>;

  using LoggerObjectPtr    = rsl::shared_ptr<rex::log::Logger>;
  using LoggerObjectPtrMap = DebugHashTable<LogCategoryName, LoggerObjectPtr>;

  bool g_enable_file_sinks = false;

  namespace logging
  {
    //-------------------------------------------------------------------------
    void init()
    {
      g_enable_file_sinks = true;
    }

    //-------------------------------------------------------------------------
    LogLevelMap get_log_levels()
    {
      // clang-format off
      return {
        {LogVerbosity::NoLogging,       rex::log::level::LevelEnum::Off},
        {LogVerbosity::Fatal,           rex::log::level::LevelEnum::Critical},
        {LogVerbosity::Error,           rex::log::level::LevelEnum::Err},
        {LogVerbosity::Warning,         rex::log::level::LevelEnum::Warn},
        {LogVerbosity::Info,            rex::log::level::LevelEnum::Info},
        {LogVerbosity::Verbose,         rex::log::level::LevelEnum::Debug},
        {LogVerbosity::VeryVerbose,     rex::log::level::LevelEnum::Trace} };
      // clang-format on
    }

    //-------------------------------------------------------------------------
    bool is_supressed(LogVerbosity verbosity)
    {
      auto level = get_log_levels()[verbosity];
      auto global_verbosity = rex::log::details::Registry::instance().get_global_level();

      return global_verbosity > level;
    }

  }

  LoggerObjectPtrMap& loggers()
  {
    static LoggerObjectPtrMap loggers(rex::global_debug_allocator());
    return loggers;
  }

  //-------------------------------------------------------------------------
  rex::log::Logger* find_logger(const LogCategoryName& name)
  {
    auto it = loggers().find(name);

    return it != rsl::cend(loggers()) ? (*it).value.get() : nullptr;
  }

  //-------------------------------------------------------------------------
  rex::log::Logger& get_logger(const LogCategory& category)
  {
    // Check if a logger with this name already exists or not
    auto logger = rex::log::details::Registry::instance().get(category.get_category_name());
    if(logger != nullptr)
      return *logger;

    // If no logger is found with the above name, create a new one
    rex::DebugVector<rsl::shared_ptr<rex::log::sinks::AbstractSink>> sinks;

    // By default we log to both the console window
#ifdef REX_ENABLE_COLOR_SINK
    // Only push rexout color sink when we are in debug mode
    sinks.push_back(rsl::allocate_shared<rex::log::sinks::StdoutColorSinkMt>(rex::global_debug_allocator()));
#endif

    // If the logging system has been initialized, we log to files as well
    if (g_enable_file_sinks)
    {
      sinks.push_back(rsl::make_shared<rex::log::sinks::basic_file_sink_mt>(rex::path::join(rex::vfs::mount_path(rex::MountingPoint::Logs), "game.log"), true));
    }

    rsl::shared_ptr<rex::log::Logger> new_logger = nullptr;

    if(category.is_async())
    {
      new_logger = rex::log::create_async<rex::log::sinks::DistSink_mt>(category.get_category_name(), rsl::move(sinks));
    }
    else
    {
      new_logger = rex::log::create<rex::log::sinks::DistSink_st>(category.get_category_name(), rsl::move(sinks));
    }

    const LogLevelMap log_levels = logging::get_log_levels();

    loggers().insert({category.get_category_name(), new_logger});

    return *find_logger(category.get_category_name());
  }

} // namespace rex