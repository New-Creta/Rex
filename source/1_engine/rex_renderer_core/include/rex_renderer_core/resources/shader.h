#pragma once

#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/system/shader_type.h"

namespace rex
{
  namespace gfx
  {
    // Base class for a shader
    class Shader
    {
    public:
      Shader(ShaderType type)
        : m_type(type)
      {}

      // Return the type of the shader
      ShaderType type() const
      {
        return m_type;
      }

    private:
      ShaderType m_type;
    };
  } // namespace gfx
} // namespace rex
