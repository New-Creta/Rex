#pragma once

#include "rex_engine/types.h"
#include "rex_std/bonus/string.h"

namespace rex
{
  struct RendererInfo
  {
    rsl::small_stack_string api_version;
    rsl::small_stack_string shader_version;
    rsl::small_stack_string adaptor;
    rsl::small_stack_string vendor;
  };

  struct RendererOutputWindowUserData
  {
    void* primary_display_handle;

    s32 window_width;
    s32 window_height;

    s32 refresh_rate;

    bool windowed;
  };

  enum class ShaderPlatform
  {
    GLSL,
    HLSL
  };

  namespace renderer
  {
    bool initialize(const RendererOutputWindowUserData& userData, u32 maxCommands);
    void shutdown();

    // general accessors
    const RendererInfo& info();
    ShaderPlatform shader_platform();

    bool is_y_up();
    bool is_depth_0_to_1();

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
      bool  	initialize(const RendererOutputWindowUserData& userData);
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