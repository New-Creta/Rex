#pragma once

#include "rex_engine/types.h"
#include "rex_std/bonus/string.h"
#include "rex_renderer_core/parameters/renderer_params.h"

namespace rex
{
  enum class ShaderPlatform
  {
    GLSL,
    HLSL
  };

  namespace renderer
  {
    struct Info
    {
        rsl::small_stack_string api_version;
        rsl::small_stack_string shader_version;
        rsl::small_stack_string adaptor;
        rsl::small_stack_string vendor;
    };

    struct OutputWindowUserData
    {
        void* primary_display_handle;

        s32 window_width;
        s32 window_height;

        s32 refresh_rate;

        bool windowed;
    };

    bool initialize(const OutputWindowUserData& userData, u32 maxCommands);
    void shutdown();

    // general accessors
    const Info& info();
    ShaderPlatform shader_platform();

    bool is_y_up();
    bool is_depth_0_to_1();

    // public-api will buffer all commands for dispatch on dedicated thread
    u32 create_clear_state(const parameters::ClearState& clearStateParams);

    void clear(u32 clearStateSlot);

    namespace backend
    {
      void*   get_device();
      void*   get_command_queue();
      void*   get_command_list();
      void*   get_command_allocator();

      s32     get_backbuffer_format();
      s32     get_depthstencil_format();

      bool    get_msaa_enabled();
      s32     get_msaa_quality();

      bool    create_pipeline_state_object(void* psoDescription);

      // Platform specific implementation, implements these function
      bool  	initialize(const OutputWindowUserData& userData);
      void  	shutdown();

      // functions to synchronize the render and main threads
      bool    new_frame();
      bool    end_frame();

      bool    flush_command_queue();

      void    clear();
      bool    present();
    } // namespace backend
  }   // namespace renderer
} // namespace rex