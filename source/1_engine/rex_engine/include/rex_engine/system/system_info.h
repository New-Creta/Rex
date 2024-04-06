#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  enum class PlatformType
  {
    Unspecified,

    Windows,
    Linux,
    Emscripten
  };

  enum class GraphicsAPIType
  {
    Unspecified,

    OpenGL,
    OpenGLES,
    DirectX12
  };

  enum class ArchitectureType
  {
    Unspecified,

    Bit32,
    Bit64
  };

  class SystemInfo
  {
  public:
    SystemInfo();

    PlatformType active_platform() const;
    GraphicsAPIType active_graphics_api() const;
    ArchitectureType active_architecture() const;

    bool is_windows() const;
    bool is_linux() const;
    bool is_emscripten() const;

    bool is_opengl() const;
    bool is_opengles() const;
    bool is_directx12() const;

    bool is_32bit() const;
    bool is_64bit() const;

  private:
    PlatformType m_platform;
    GraphicsAPIType m_graphics_api;
    ArchitectureType m_architecture;
  };

  const SystemInfo& system_info();

  // ******************************************************
  // PLATFORM SPECIFIC IMPLEMENTATION
  // ******************************************************
  namespace sys_info
  {
    // Returns the number of physical cores on the system
    s32 number_physical_cores();

    // Returns the number of logical processors
    s32 number_logical_processors();
  } // namespace sys_info
} // namespace rex

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/system/win_sys_info.h"
#endif