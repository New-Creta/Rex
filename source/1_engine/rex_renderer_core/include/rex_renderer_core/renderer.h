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
      // Platform specific implementation, implements these function
      bool initialize(const RendererOutputWindowUserData& userData);
      void shutdown();

      // functions to synchronize the render and main threads
      void new_frame();
      void end_frame();

      void clear();
      void present();
    } // namespace backend
  }   // namespace renderer
} // namespace rex