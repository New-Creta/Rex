#include "rex_engine/defines.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/iostream.h"
#include "rex_std_extra/utility/type_id.h"
#include "rex_windows/gui_application.h"
#include "rex_engine/event_system.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRegina, LogVerbosity::Log);
DEFINE_LOG_CATEGORY(LogRegina);

namespace rex
{
  void process_input(char input)
  {
    if (input == 'w')
    {
      REX_MEM_TAG_SCOPE(MemoryTag::Windows);
      [[maybe_unused]] int* p = new int(2);
    }
    else if (input == 'e')
    {
      const MemoryUsageStats mem_stats = rex::mem_tracker().current_stats();

      for (const auto mem_tag : rsl::enum_refl::enum_entries<MemoryTag>())
      {
        REX_LOG(LogRegina, "{} : {} bytes", mem_tag.name(), mem_stats.usage_per_tag[rsl::enum_refl::enum_integer(mem_tag.val())]);
      }

      for (const MemoryHeader* header : mem_stats.allocation_headers)
      {
        REX_LOG(LogRegina, "ptr - {}", header->ptr());
        REX_LOG(LogRegina, "size - {}", header->size());
        REX_LOG(LogRegina, "tag - {}", rsl::enum_refl::enum_name(header->tag()));
        REX_LOG(LogRegina, "thread id - {}", header->thread_id());
        REX_LOG(LogRegina, "frame index - {}", header->frame_index());
        REX_LOG(LogRegina, "callstack");
        //REX_LOG(LogRegina, header->callstack());
      }
    }
  }

  bool initialize()
  {
    REX_LOG(LogRegina, "Initializing Regina");

    event_system::subscribe(event_system::EventType::CharDown, [](void* input) { process_input(*reinterpret_cast<char*>(input)); });
   

    return true;
  }
  void update()
  {
    // REX_INFO("updating Regina");
  }
  void shutdown()
  {
    REX_LOG(LogRegina, "shutting down Regina");
  }

  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs)
  {
    ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina";

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_shutdown_func = shutdown;

    return app_params;
  }
} // namespace rex
