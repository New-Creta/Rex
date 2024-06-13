#pragma once

namespace rex
{
  namespace renderer
  {
    enum class ShaderPlatform
    {
      Glsl, // GLSL shader platform (used for OpenGL and Vulkan)
      Hlsl  // HLSL shader platform (used for DirectX)
    };
  } // namespace renderer
} // namespace rex