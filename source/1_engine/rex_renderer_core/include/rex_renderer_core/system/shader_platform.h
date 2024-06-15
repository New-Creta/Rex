#pragma once

namespace rex
{
  namespace gfx
  {
    enum class ShaderPlatform
    {
      Glsl, // GLSL shader platform (used for OpenGL and Vulkan)
      Hlsl  // HLSL shader platform (used for DirectX)
    };
  } // namespace gfx
} // namespace rex