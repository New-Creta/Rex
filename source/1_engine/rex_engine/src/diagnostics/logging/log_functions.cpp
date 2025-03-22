#include "rex_engine/diagnostics/logging/log_functions.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/internal/details/diag_thread_pool.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/logger_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/basic_file_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/dist_sink.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_color_sinks.h"
#include "rex_engine/engine/debug_types.h"
#include "rex_engine/engine/project.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/memory/global_allocators/global_allocator.h"
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
  using LogLevelMap = debug_hash_map<log::LogVerbosity, rex::log::level::LevelEnum>;

  using LoggerObjectPtr    = rsl::shared_ptr<rex::log::Logger>;
  using LoggerObjectPtrMap = debug_hash_map<LogCategoryName, LoggerObjectPtr>;

  bool g_enable_file_sinks = false; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  namespace log
  {
    //-------------------------------------------------------------------------
    void init_log_verbosity()
    {      
      LogVerbosity verbosity = LogVerbosity::Info;
      if (rex::is_debugger_attached())
      {
        verbosity = LogVerbosity::Debug;
      }

      // read the required log versboity from the commandline
      rsl::optional<rsl::string_view> log_level = cmdline::get_argument("LogVerbosity");
      if(log_level)
      {
        verbosity = rsl::enum_refl::enum_cast<LogVerbosity>(log_level.value()).value_or(verbosity);
      }

      // assign the verbosity as the global verbosity log level
      rex::log::details::Registry::instance().set_level(verbosity);
    }

    //-------------------------------------------------------------------------
    void init()
    {
      // shutdown the logging registry, flushing all loggers
      // We'll be starting from scratch again, as we now have all systems
      // initialized to start proper logging with the engine
      rex::log::details::Registry::instance().shutdown();

      // Initialize the log verbosity as early as possible
      // They don't have dependencies (other than the commandline)
      // and are pretty much required by everything else
      // Log verbosity determines how much we log.
      init_log_verbosity();

      // Setup the path where logs will go using a mounting point
      // With mounting points, we abstract away the hardcoded paths
      // so we can always set them elsewhere at runtime if we want to
      rex::vfs::mount(rex::MountingPoint::Logs, path::join(vfs::current_session_root(), "logs"));

      // Enable file sinks. The vfs is initialized by now, and the path for logs as well
      // we can start using file sinks
      g_enable_file_sinks = true;
    }

    //-------------------------------------------------------------------------
    bool is_supressed(LogVerbosity verbosity)
    {
      auto global_verbosity = rex::log::details::Registry::instance().get_global_level();
      return global_verbosity > verbosity;
    }

    //-------------------------------------------------------------------------
    rex::log::Logger& get_logger(const LogCategory& category)
    {
      // Check if a logger with this name already exists or not
      auto logger = rex::log::details::Registry::instance().get(category.get_category_name());
      if(logger != nullptr)
        return *logger;

      // If no logger is found with the above name, create a new one
      rex::debug_vector<rsl::shared_ptr<rex::log::sinks::AbstractSink>> sinks;

      // By default we log to both the console window
#ifdef REX_ENABLE_COLOR_SINK
      // Only push rexout color sink when we are in debug mode
      sinks.push_back(rsl::allocate_shared<rex::log::sinks::StdoutColorSinkMt>(rex::GlobalDebugAllocator()));
#endif

      // If the logging system has been initialized, we log to files as well
      if(g_enable_file_sinks)
      {
        sinks.push_back(rsl::allocate_shared<rex::log::sinks::basic_file_sink_mt>(rex::GlobalDebugAllocator(), rex::project_log_path(), true));
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

      return *new_logger;
    }
  } // namespace log

} // namespace rex