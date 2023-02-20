#include "rex_engine/system_info.h"

namespace rex
{
  //-------------------------------------------------------------------------
  SystemInfo::SystemInfo()
  {
#if defined REX_PLATFORM_WINDOWS
    m_platform = platform::Type::Windows;
#elif defined REX_PLATFORM_LINUX
    m_platform     = platform::Type::Linux;
#elif defined REX_PLATFORM_EMSCRIPTEN
    m_platform     = platform::Type::Emscripten;
#else
    m_platform     = platform::Type::Unspecified;
#endif

#if defined REX_PLATFORM_WINDOWS
  #if defined REX_API_OPENGL
    m_graphics_api = graphics_api::Type::OpenGL;
  #elif defined REX_API_OPENGLES
    m_graphics_api = graphics_api::Type::OpenGLES;
  #elif defined REX_API_DIRECTX12
    m_graphics_api = graphics_api::Type::DirectX12;
  #else
    m_graphics_api = graphics_api::Type::Unspecified;
  #endif
#elif defined REX_PLATFORM_LINUX
  #if defined REX_API_OPENGL
    m_graphics_api = graphics_api::Type::OpenGL;
  #else
    m_graphics_api = graphics_api::Type::Unspecified;
  #endif
#elif defined REX_PLATFORM_EMSCRIPTEN
  #if defined REX_API_OPENGLES
    m_graphics_api = graphics_api::Type::OpenGLES;
  #else
    m_graphics_api = graphics_api::Type::Unspecified;
  #endif
#else
    m_graphics_api = graphics_api::Type::Unspecified;
#endif

    constexpr int c_bits_32 = 4;
    constexpr int c_bits_64 = 8;
    switch(sizeof(void*))
    {
      case c_bits_32: m_architecture = architecture::Type::Bit32; break;
      case c_bits_64: m_architecture = architecture::Type::Bit64; break;
      default: m_architecture = architecture::Type::Unspecified; break;
    }
  }

  //-------------------------------------------------------------------------
  platform::Type SystemInfo::get_active_platform()
  {
    return m_platform;
  }
  //-------------------------------------------------------------------------
  graphics_api::Type SystemInfo::get_active_graphics_api()
  {
    return m_graphics_api;
  }
  //-------------------------------------------------------------------------
  architecture::Type SystemInfo::get_active_architecture()
  {
    return m_architecture;
  }

  //-------------------------------------------------------------------------
  rsl::array<platform::Type, 3> SystemInfo::get_supported_platforms()
  {
    return {platform::Type::Windows, platform::Type::Linux, platform::Type::Emscripten};
  }
  //-------------------------------------------------------------------------
  rsl::array<graphics_api::Type, 3> SystemInfo::get_supported_graphics_apis()
  {
    return {graphics_api::Type::OpenGL, graphics_api::Type::OpenGLES, graphics_api::Type::DirectX12};
  }
  //-------------------------------------------------------------------------
  rsl::array<architecture::Type, 2> SystemInfo::get_supported_architectures()
  {
    return {architecture::Type::Bit32, architecture::Type::Bit64};
  }

  //-------------------------------------------------------------------------
  bool SystemInfo::is_windows()
  {
    return get_active_platform() == platform::Type::Windows;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_linux()
  {
    return get_active_platform() == platform::Type::Linux;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_emscripten()
  {
    return get_active_platform() == platform::Type::Emscripten;
  }

  //-------------------------------------------------------------------------
  bool SystemInfo::is_opengl()
  {
    return get_active_graphics_api() == graphics_api::Type::OpenGL;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_opengles()
  {
    return get_active_graphics_api() == graphics_api::Type::OpenGLES;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_directx12()
  {
    return get_active_graphics_api() == graphics_api::Type::DirectX12;
  }

  //-------------------------------------------------------------------------
  bool SystemInfo::is_32bit()
  {
    return get_active_architecture() == architecture::Type::Bit32;
  }
  //-------------------------------------------------------------------------
  bool SystemInfo::is_64bit()
  {
    return get_active_architecture() == architecture::Type::Bit64;
  }

  //-------------------------------------------------------------------------
  const SystemInfo& get_system_info()
  {
    static SystemInfo s_system_info;

    return s_system_info;
  }
} // namespace rex