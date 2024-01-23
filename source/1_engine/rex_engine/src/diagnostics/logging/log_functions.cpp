#include "rex_engine/diagnostics/logging/log_functions.h"

#include "rex_engine/engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/details/thread_pool.h"
#include "rex_engine/diagnostics/logging/internal/logger_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/basic_file_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/dist_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_color_sinks.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/vector.h"


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

  LoggerObjectPtrMap& loggers()
  {
    static LoggerObjectPtrMap loggers(rex::global_debug_allocator());
    return loggers;
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
      {LogVerbosity::Log,             rex::log::level::LevelEnum::Info},      
      {LogVerbosity::Verbose,         rex::log::level::LevelEnum::Debug},  
      {LogVerbosity::VeryVerbose,     rex::log::level::LevelEnum::Trace}};
    // clang-format on
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
    const LogLevelMap log_levels = get_log_levels();

    // assert(LOG_LEVELS.find(category.get_verbosity()) != rsl::cend(LOG_LEVELS) && "Unknown log verbosity was given");

    auto logger = rex::log::details::Registry::instance().get(category.get_category_name());
    if(logger != nullptr)
      return *logger;

    // rsl::filesystem::path working_dir(rsl::filesystem::current_path());
    // rsl::filesystem::path log_dir("log");
    // rsl::filesystem::path filename(category.get_category_name().data());
    // rsl::filesystem::path full_path = working_dir / log_dir / filename;

    rex::DebugVector<rsl::shared_ptr<rex::log::sinks::AbstractSink>> sinks;

#ifdef REX_ENABLE_COLOR_SINK
    // Only push rexout color sink when we are in debug mode
    sinks.push_back(rsl::allocate_shared<rex::log::sinks::StdoutColorSinkMt>(rex::global_debug_allocator()));
#endif
    // sinks.push_back(rsl::make_shared<rex::log::sinks::basic_file_sink_mt>(full_path.string(), true));

    rsl::shared_ptr<rex::log::Logger> new_logger = nullptr;

    if(category.is_async())
    {
      new_logger = rex::log::create_async<rex::log::sinks::DistSink_mt>(category.get_category_name(), rsl::move(sinks));
    }
    else
    {
      new_logger = rex::log::create<rex::log::sinks::DistSink_st>(category.get_category_name(), rsl::move(sinks));
    }

    new_logger->set_level(log_levels.at(category.get_verbosity()));

    loggers().insert({category.get_category_name(), new_logger});

    return *find_logger(category.get_category_name());
  }

} // namespace rex