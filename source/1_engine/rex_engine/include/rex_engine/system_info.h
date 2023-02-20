#pragma once

#include "rex_std/array.h"

namespace rex
{
  namespace platform
  {
    enum class Type
    {
      Unspecified,

      Windows,
      Linux,
      Emscripten
    };
  } // namespace platform

  namespace graphics_api
  {
    enum class Type
    {
      Unspecified,

      OpenGL,
      OpenGLES,
      DirectX12
    };
  } // namespace graphics_api

  namespace architecture
  {
    enum class Type
    {
      Unspecified,

      Bit32,
      Bit64
    };
  } // namespace architecture

  class SystemInfo
  {
  public:
    SystemInfo();

    platform::Type get_active_platform();
    graphics_api::Type get_active_graphics_api();
    architecture::Type get_active_architecture();

    rsl::array<platform::Type, 3> get_supported_platforms();
    rsl::array<graphics_api::Type, 3> get_supported_graphics_apis();
    rsl::array<architecture::Type, 2> get_supported_architectures();

    bool is_windows();
    bool is_linux();
    bool is_emscripten();

    bool is_opengl();
    bool is_opengles();
    bool is_directx12();

    bool is_32bit();
    bool is_64bit();

  private:
    platform::Type m_platform;
    graphics_api::Type m_graphics_api;
    architecture::Type m_architecture;
  };

  const SystemInfo& get_system_info();
} // namespace rex