#pragma once

#include "rex_engine/types.h"
#include "rex_std/bonus/string/stack_string.h"

namespace rex
{
  struct RendererInfo
  {
    rsl::stack_string<u8, 64> api_version;
    rsl::stack_string<u8, 64> shader_version;
    rsl::stack_string<u8, 64> adaptor;
    rsl::stack_string<u8, 64> vendor;
  };

  enum class ShaderPlatform
  {
    GLSL,
    HLSL
  };

  namespace renderer
  {
    bool initialize(void* userData, u32 maxCommands);
    void shutdown();

    // general accessors
    const RendererInfo& get_info();
    ShaderPlatform shader_platform();

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

      void clear();
      void present();
    } // namespace backend
  }   // namespace renderer
} // namespace rex