#include "rex_engine/system_info.h"

// NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer,-warnings-as-errors)

namespace rex
{
  //-------------------------------------------------------------------------
  SystemInfo::SystemInfo()
  {
#if defined REX_PLATFORM_WINDOWS
    m_platform = PlatformType::Windows;
#elif defined REX_PLATFORM_LINUX
    m_platform = PlatformType::Linux;
#elif defined REX_PLATFORM_EMSCRIPTEN
    m_platform = PlatformType::Emscripten;
#else
    m_platform = PlatformType::Unspecified;
#endif

#if defined REX_PLATFORM_WINDOWS
  #if defined REX_API_OPENGL
    m_graphics_api = GraphicsAPIType::OpenGL;
  #elif defined REX_API_OPENGLES
    m_graphics_api = GraphicsAPIType::OpenGLES;
  #elif defined REX_API_DIRECTX12
    m_graphics_api = GraphicsAPIType::DirectX12;
  #else
    m_graphics_api = GraphicsAPIType::Unspecified;
  #endif
#elif defined REX_PLATFORM_LINUX
  #if defined REX_API_OPENGL
    m_graphics_api = GraphicsAPIType::OpenGL;
  #else
    m_graphics_api = GraphicsAPIType::Unspecified;
  #endif
#elif defined REX_PLATFORM_EMSCRIPTEN
  #if defined REX_API_OPENGLES
    m_graphics_api = GraphicsAPIType::OpenGLES;
  #else
    m_graphics_api = GraphicsAPIType::Unspecified;
  #endif
#else
    m_graphics_api = GraphicsAPIType::Unspecified;
#endif

    constexpr int c_bits_32 = 4;
    constexpr int c_bits_64 = 8;
    switch(sizeof(void*))
    {
      case c_bits_32: m_architecture = ArchitectureType::Bit32; break;
      case c_bits_64: m_architecture = ArchitectureType::Bit64; break;
      default: m_architecture = ArchitectureType::Unspecified; break;
    }
  }

  //-------------------------------------------------------------------------
  PlatformType SystemInfo::active_platform() const
  {
    return m_platform;
  }
  //-------------------------------------------------------------------------
  GraphicsAPIType SystemInfo::active_graphics_api() const
  {
    return m_graphics_api;
  }
  //-------------------------------------------------------------------------
  ArchitectureType SystemInfo::active_architecture() const
  {
    return m_architecture;
  }

  //-------------------------------------------------------------------------
  bool SystemInfo::is_windows() const
  {
    return active_platform() == PlatformType::Windows;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_linux() const
  {
    return active_platform() == PlatformType::Linux;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_emscripten() const
  {
    return active_platform() == PlatformType::Emscripten;
  }

  //-------------------------------------------------------------------------
  bool SystemInfo::is_opengl() const
  {
    return active_graphics_api() == GraphicsAPIType::OpenGL;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_opengles() const
  {
    return active_graphics_api() == GraphicsAPIType::OpenGLES;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_directx12() const
  {
    return active_graphics_api() == GraphicsAPIType::DirectX12;
  }

  //-------------------------------------------------------------------------
  bool SystemInfo::is_32bit() const
  {
    return active_architecture() == ArchitectureType::Bit32;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_64bit() const
  {
    return active_architecture() == ArchitectureType::Bit64;
  }

  //-------------------------------------------------------------------------
  const SystemInfo& system_info()
  {
    static SystemInfo s_system_info;

    return s_system_info;
  }
} // namespace rex

// NOLINTEND(modernize-use-nullptr,-warnings-as-errors)