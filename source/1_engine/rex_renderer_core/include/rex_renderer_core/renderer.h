#pragma once

#include "rex_engine/types.h"

namespace rex
{
  struct RendererInfo
  {
    const char8* api_version;
    const char8* shader_version;
    const char8* renderer;
    const char8* vendor;
  };

  namespace renderer
  {
    bool initialize(void* userData, u32 maxCommands);
    void shutdown();

    // general accessors
    const RendererInfo& get_info();
    const c8* shader_platform();

    bool is_y_up();
    bool is_depth_0_to_1();

    namespace backend
    {
        // Platform specific implementation, implements these function
        bool initialize();
        void shutdown();

        // functions to synchronize the render and main threads
        void new_frame();
        void end_frame();

        // clears
        void clear();

        // swap / present / vsync
        void present();
    }
  } // namespace renderer
} // namespace rex