#include "rex_engine/diagnostics/logging/logger.h"

#include "rex_engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/sinks/basic_file_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_color_sinks.h"
#include "rex_engine/memory/debug_allocator.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/memory/untracked_allocator.h"
#include "rex_std/filesystem.h"
#include "rex_std/unordered_map.h"
#include "rex_std/utility.h"
#include "rex_std/vector.h"

namespace rex
{
  using LogPattern  = const char*;
  using LogLevelMap = DebugHashTable<LogVerbosity, rexlog::level::LevelEnum>;

  using LoggerObjectPtr    = rsl::shared_ptr<rexlog::Logger>;
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
      {LogVerbosity::NoLogging,       rexlog::level::Off}, 
      {LogVerbosity::Fatal,           rexlog::level::Critical}, 
      {LogVerbosity::Error,           rexlog::level::Err},        
      {LogVerbosity::Warning,         rexlog::level::Warn},
      {LogVerbosity::Log,             rexlog::level::Info},      
      {LogVerbosity::Verbose,         rexlog::level::Debug},  
      {LogVerbosity::VeryVerbose,     rexlog::level::Trace}};
    // clang-format on
  }

  //-------------------------------------------------------------------------
  rexlog::Logger* find_logger(const LogCategoryName& name)
  {
    auto it = loggers().find(name);

    return it != rsl::cend(loggers()) ? (*it).value.get() : nullptr;
  }

  //-------------------------------------------------------------------------
  rexlog::Logger& get_logger(const LogCategory& category)
  {
    const LogPattern default_pattern = "%^[%T][%=8l] %n: %v%$";
    const LogLevelMap log_levels     = get_log_levels();

    // assert(LOG_LEVELS.find(category.get_verbosity()) != rsl::cend(LOG_LEVELS) && "Unknown log verbosity was given");

    rexlog::Logger* logger = find_logger(category.get_category_name());
    if(logger != nullptr)
      return *logger;

    // rsl::filesystem::path working_dir(rsl::filesystem::current_path());
    // rsl::filesystem::path log_dir("log");
    // rsl::filesystem::path filename(category.get_category_name().data());
    // rsl::filesystem::path full_path = working_dir / log_dir / filename;

    rex::DebugVector<rexlog::sink_ptr> sinks;

#if REX_DEBUG
    // Only push rexout color sink when we are in debug mode
    sinks.push_back(rsl::allocate_shared<rexlog::sinks::stdout_color_sink_mt>(rex::global_debug_allocator()));
#endif
    // sinks.push_back(rsl::make_shared<rexlog::sinks::basic_file_sink_mt>(full_path.string(), true));

    rsl::shared_ptr<rexlog::Logger> new_logger = rsl::allocate_shared<rexlog::Logger>(rex::global_debug_allocator(), rex::DebugString(category.get_category_name()), rsl::begin(sinks), rsl::end(sinks));

    new_logger->set_pattern(rsl::basic_string<char8, rsl::char_traits<char8>, DebugAllocator<UntrackedAllocator>>(default_pattern, global_debug_allocator()));
    new_logger->set_level(log_levels.at(category.get_verbosity()));

    loggers().insert({category.get_category_name(), new_logger});

    return *find_logger(category.get_category_name());
  }

} // namespace rex