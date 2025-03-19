#pragma once

#include "rex_engine/types.h"
#include "rex_opengl/resources/resource.h"
#include "rex_std/bonus/string.h"
#include "rex_std/unordered_map.h"

#ifdef REX_COMPILER_MSVC
  #pragma warning(push)
  #pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#endif

#include <glm/glm.hpp>

#ifdef REX_COMPILER_MSVC
  #pragma warning(pop)
#endif

namespace rex
{
  class ShaderUniformLocation
  {
  public:
    //-------------------------------------------------------------------------
    ShaderUniformLocation()
        : m_location(-1)
    {
    }
    //-------------------------------------------------------------------------
    explicit ShaderUniformLocation(s32 loc)
        : m_location(loc)
    {
    }

    //-------------------------------------------------------------------------
    explicit operator bool() const
    {
      return m_location != -1;
    }
    //-------------------------------------------------------------------------
    explicit operator s32() const
    {
      return m_location;
    }

    //-------------------------------------------------------------------------
    s32 get() const
    {
      return m_location;
    }

  private:
    s32 m_location;
  };

  // Uniform Location Map = <Name of the Uniform, Location of the Uniform>
  using UniformLocationMap = rsl::unordered_map<rsl::small_stack_string, ShaderUniformLocation>;

  class ShaderProgram : public Resource
  {
  public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode);
    ~ShaderProgram() override;

    ShaderProgram& operator=(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    u32 resource_id() const override;

    void bind() const override;
    void unbind() const override;

    void release() override;

    bool set_uniform(const rsl::small_stack_string& name, const glm::mat3& mat);
    bool set_uniform(const rsl::small_stack_string& name, const glm::mat4& mat);

    bool set_uniform(const rsl::small_stack_string& name, float value);
    bool set_uniform(const rsl::small_stack_string& name, const glm::vec2& value);
    bool set_uniform(const rsl::small_stack_string& name, const glm::vec3& value);
    bool set_uniform(const rsl::small_stack_string& name, const glm::vec4& value);

    bool set_uniform(const rsl::small_stack_string& name, const glm::ivec2& value);
    bool set_uniform(const rsl::small_stack_string& name, const glm::ivec3& value);
    bool set_uniform(const rsl::small_stack_string& name, const glm::ivec4& value);

    bool set_uniform(const rsl::small_stack_string& name, const float* values, u32 size);

    bool set_uniform(const rsl::small_stack_string& name, s32 value);
    bool set_uniform(const rsl::small_stack_string& name, u32 value);
    bool set_uniform(const rsl::small_stack_string& name, bool value);

  private:
    u32 m_resource_id;
    UniformLocationMap m_uniform_locations;
    mutable bool m_bound;
  };
} // namespace rex