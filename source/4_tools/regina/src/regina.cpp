#include "rex_engine/defines.h"
#include "rex_engine/entrypoint.h"
#include "rex_windows/gui_application.h"

#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/thread.h"

namespace rex
{
  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs)
  {
    int* p = nullptr;
    int* q = nullptr;
    rsl::thread t1([&]() 
      {
        MEM_TAG_SCOPE(MemoryTag::Engine);
        p = new int(3);
      });
    t1.join();
    rsl::thread t2([&]() 
      {
        MEM_TAG_SCOPE(MemoryTag::Editor);
        delete p;
        q = new int(4);
      });
    t2.join();
    rsl::thread t3([&]() 
      {
        MEM_TAG_SCOPE(MemoryTag::Engine);
        delete q;
      });
    t3.join();
    ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina";
    
    app_params.engine_params.max_memory = 256_kb;

    return app_params;
  }
} // namespace rex
