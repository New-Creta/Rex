#include "rex_engine/engine/defines.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/iostream.h"
#include "rex_std/bonus/utility/type_id.h"
#include "rex_windows/app/console_application.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/memory/memory_header.h"
#include "rex_windows/input/input.h"
#include "rex_engine/diagnostics/stacktrace.h"

DEFINE_LOG_CATEGORY(LogConsoleApp);

namespace console_app_example
{
  void pointer_tracking_test()
  {
    // the following pointer should get tracked and automatically be processed by the tracking system
    // depending on which one is enabled

    // the code will follow the following step:
    // operator new -> Global Allocator (Tracked) -> Global Allocator (Untracked) -> System Call
    //                                                                                    |
    //                                                                                    | 
    // operator new <- Global Allocator (Tracked) <- Global Allocator (Untracked)  <------+ 
    //                         |
    //                         |
    //                         v
    //                   Save to file
    // 
    // Functionality should be implemented in memory_tracking.cpp
    // However, we'll need a file system first to save things to disk
    // We can then write a tool that scans this file for a memory analysis
    // 
    // Requirements of this tool:
    // - Open a memory report file saved by the engine (See above)
    // - Track all live allocations with their callstacks
    // - Diff between different points in time
    // - Track all single frame allocations

    rsl::unique_ptr<int> p = rsl::make_unique<int>(1);
    (void)p.release();
  }

  void display_mem_usage_stats(const rex::MemoryUsageStats& stats)
  {
    for (count_t i = 0; i < stats.usage_per_tag.size(); ++i)
    {
      rex::MemoryTag tag = static_cast<rex::MemoryTag>(i);
      REX_INFO(LogConsoleApp, "{}: {} bytes", rsl::enum_refl::enum_name(tag), stats.usage_per_tag[i]);
    }

    REX_INFO(LogConsoleApp, "----------------------------");

    for (rex::MemoryHeader* header : stats.allocation_headers)
    {
      rex::ResolvedCallstack callstack(header->callstack());

      REX_INFO(LogConsoleApp, "Frame: {}", header->frame_index());
      REX_INFO(LogConsoleApp, "Thread ID: {}", header->thread_id());
      REX_INFO(LogConsoleApp, "Memory Tag: {}", rsl::enum_refl::enum_name(header->tag()));
      REX_INFO(LogConsoleApp, "Size: {}", header->size());

      for (count_t i = 0; i < callstack.size(); ++i)
      {
        REX_INFO(LogConsoleApp, "{}", callstack[i]);
      }
    }

    REX_INFO(LogConsoleApp, "Total of {} allocations", stats.allocation_headers.size());
  }

  bool initialize()
  {
    return true;
  }
  void update()
  {
    if (rex::input::is_key_pressed('P'))
    {
      rex::mem_tracker().dump_stats_to_file("mem_stats.txt");
    }

    if (rex::input::is_key_pressed('Q'))
    {
      rex::event_system::enqueue_event(rex::event_system::EventType::QuitApp);
    }
  }
  void shutdown()
  {
  }

} // namespace console_app_example

namespace rex
{
  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
  {
    ApplicationCreationParams app_params(&platformParams);

    app_params.engine_params.app_init_func     = console_app_example::initialize;
    app_params.engine_params.app_update_func   = console_app_example::update;
    app_params.engine_params.app_shutdown_func = console_app_example::shutdown;

    return app_params;
  }
}
