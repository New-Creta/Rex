#pragma once

#include "rex_engine/types.h"
#include "rex_std/bonus/attributes.h"

namespace rex
{
  enum class ShaderType;

  class Shader
  {
  public:
    Shader(const ShaderType& inType, const char** shaderElements, s32* shaderElementLength = nullptr);
    ~Shader();

    u32 get_resource_id() const;

    void release();

  private:
    u32 m_resource_id;
  };

  Shader create_vertex_shader(const char** shaderElements, s32* shaderElementLength = nullptr);
  Shader create_fragment_shader(const char** shaderElements, s32* shaderElementLength = nullptr);
}