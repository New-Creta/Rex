#include "rex_opengl/resources/shader_program.h"

#include "rex_engine/diagnostics/legacy/logging.h"
#include "rex_opengl/opengl_error.h"
#include "rex_opengl/resources/shader.h"
#include "rex_std/sstream.h"

#if REX_PLATFORM_X64
  #include <glad/gl.h>
#else
  #error "Unsupported platform"
#endif

#if defined REX_COMPILER_MSVC
  #pragma warning(push)
  #pragma warning(disable : 4018) // 'token' : signed/unsigned mismatch
  #pragma warning(disable : 4100) // 'identifier' : unreferenced formal parameter
  #pragma warning(disable : 4127) // conditional expression is constant
  #pragma warning(disable : 4189) // 'identifier' : local variable is initialized but not referenced
  #pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
  #pragma warning(disable : 4244) // 'conversion' conversion from 'type1' to 'type2', possible loss of data
  #pragma warning(disable : 4245) // 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch
  #pragma warning(disable : 4267) // 'var' : conversion from 'size_t' to 'type', possible loss of data
  #pragma warning(disable : 4389) // 'equality-operator' : signed/unsigned mismatch
  #pragma warning(disable : 4456) // declaration of 'identifier' hides previous local declaration
  #pragma warning(disable : 4457) // declaration of 'identifier' hides function parameter
  #pragma warning(disable : 4458) // declaration of 'identifier' hides class member
  #pragma warning(disable : 4459) // declaration of 'identifier' hides global declaration
  #pragma warning(disable : 4701) // Potentially uninitialized local variable 'name' used
  #pragma warning(disable : 4996) // Your code uses a function, class member, variable, or typedef that's marked deprecated.
#endif

#include <glm/gtc/type_ptr.hpp>

#if defined REX_COMPILER_MSVC
  #pragma warning(pop)
#endif

namespace rex
{
  namespace shader_program
  {
    //-------------------------------------------------------------------------
    bool find_and_cache_uniform(const u32& programID, const rsl::small_stack_string& inName, rsl::unordered_map<rsl::small_stack_string, ShaderUniformLocation>& uniformLocations)
    {
      if(uniformLocations.find(inName) == rsl::end(uniformLocations))
      {
        REX_TRACE("Caching uniform: {0}, for shader program with ID: {1}", inName, programID);

        const s32 location       = glGetUniformLocation(programID, inName.c_str());
        uniformLocations[inName] = ShaderUniformLocation(location);

        if(location == -1)
        {
          REX_WARN("Uniform with name: \"{0}\" was not found", inName);
          return false;
        }
      }

      return true;
    }
  } // namespace shader_program

  //-----------------------------------------------------------------------
  ShaderProgram::ShaderProgram()
      : m_resource_id(0)
      , m_bound(false)
  {
  }
  //-----------------------------------------------------------------------
  ShaderProgram::ShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode)
      : m_resource_id(0)
      , m_bound(false)
  {
    const Shader vertex_shader   = create_vertex_shader(&vertexShaderCode);
    const Shader fragment_shader = create_fragment_shader(&fragmentShaderCode);

    GL_CALL(m_resource_id = glCreateProgram()); // NOLINT (cppcoreguidelines-prefer-member-initializer,-warnings-as-errors)

    glAttachShader(m_resource_id, vertex_shader.resource_id());
    glAttachShader(m_resource_id, fragment_shader.resource_id());

    GL_CALL(glLinkProgram(m_resource_id));

    s32 success = 0;
    GL_CALL(glGetProgramiv(m_resource_id, GL_LINK_STATUS, &success));

    if(success == 0)
    {
      rsl::array<char, 512> info_log;
      GL_CALL(glGetProgramInfoLog(m_resource_id, 512, nullptr, info_log.data()));

      REX_ERROR(rsl::string(info_log.data()));
      REX_ERROR("Could not link shader program");

      m_resource_id = 0;

      return;
    }
  }

  //-------------------------------------------------------------------------
  ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
      : m_resource_id(rsl::exchange(other.m_resource_id, 0))
      , m_uniform_locations(rsl::exchange(other.m_uniform_locations, rsl::unordered_map<rsl::small_stack_string, ShaderUniformLocation>()))
      , m_bound(rsl::exchange(other.m_bound, false))
  {
  }

  //-----------------------------------------------------------------------
  ShaderProgram::~ShaderProgram()
  {
    release();
  }

  //-------------------------------------------------------------------------
  rex::ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
  {
    REX_ASSERT_X(this != &other, "can't move to yourself");

    m_resource_id       = rsl::exchange(other.m_resource_id, 0);
    m_uniform_locations = rsl::exchange(other.m_uniform_locations, rsl::unordered_map<rsl::small_stack_string, ShaderUniformLocation>());
    m_bound             = rsl::exchange(other.m_bound, false);

    return *this;
  }

  //-----------------------------------------------------------------------
  u32 ShaderProgram::resource_id() const
  {
    return m_resource_id;
  }

  //-----------------------------------------------------------------------
  void ShaderProgram::bind() const
  {
    REX_ASSERT_X(m_resource_id != 0, "Binding an invalid resource id");

    GL_CALL(glUseProgram(m_resource_id));

    m_bound = true;
  }
  //-----------------------------------------------------------------------
  void ShaderProgram::unbind() const
  {
    GL_CALL(glUseProgram(0));

    m_bound = false;
  }

  //-----------------------------------------------------------------------
  void ShaderProgram::release()
  {
    GL_CALL(glDeleteProgram(m_resource_id));

    m_resource_id = 0;
    m_bound       = false;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::mat3& mat)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniformMatrix3fv(location, 1, false, glm::value_ptr(mat)));

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::mat4& mat)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniformMatrix4fv(location, 1, false, glm::value_ptr(mat)));

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const float value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform1f(location, value));

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::vec2& value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform2f(location, value.x, value.y)); // NOLINT (cppcoreguidelines-pro-type-union-access,-warnings-as-errors)

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::vec3& value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform3f(location, value.x, value.y, value.z)); // NOLINT (cppcoreguidelines-pro-type-union-access,-warnings-as-errors

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::vec4& value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform4f(location, value.x, value.y, value.z, value.w)); // NOLINT (cppcoreguidelines-pro-type-union-access,-warnings-as-errors

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::ivec2& value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform2i(location, value.x, value.y)); // NOLINT (cppcoreguidelines-pro-type-union-access,-warnings-as-errors

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::ivec3& value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform3i(location, value.x, value.y, value.z)); // NOLINT (cppcoreguidelines-pro-type-union-access,-warnings-as-errors

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const glm::ivec4& value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform4i(location, value.x, value.y, value.z, value.w)); // NOLINT (cppcoreguidelines-pro-type-union-access,-warnings-as-errors

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const float* values, u32 size)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform1fv(location, size, values));

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const s32 value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform1i(location, value));

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const u32 value)
  {
    if(!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
    {
      REX_WARN("Uniform ({0}) not found on shader.", name);
      return false;
    }

    const s32 location = static_cast<s32>(m_uniform_locations.at(name));
    GL_CALL(glUniform1ui(location, value));

    return true;
  }

  //-----------------------------------------------------------------------
  bool ShaderProgram::set_uniform(const rsl::small_stack_string& name, const bool value)
  {
    return set_uniform(name, value ? 1 : 0);
  }

} // namespace rex