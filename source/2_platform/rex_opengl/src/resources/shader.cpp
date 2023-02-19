#include "rex_opengl/resources/shader.h"

#include "rex_engine/diagnostics/logging.h"

#define REX_ENABLE_STD_ALIAS
#include "rex_std/sstream.h"
#include "rex_std_extra/utility/casting.h"

#if REX_PLATFORM_X64
  #include <glad/gl.h>
#else
  #error "Unsupported platform"
#endif

REX_STATIC_TODO("sstream define REX_ENABLE_STD_ALIAS(stringstream) is not working properly, adding workaround here");

namespace rsl
{
  inline namespace v1
  {
    using stringstream = std::stringstream;
  } // namespace v1
} // namespace rsl

namespace rex
{
  namespace conversions
  {
    //-----------------------------------------------------------------------
    rsl::string_view to_string(u32 shaderType)
    {
      switch(shaderType)
      {
        case GL_VERTEX_SHADER: return "Vertex Shader";
        case GL_FRAGMENT_SHADER: return "Fragment Shader";
        default:
          // Nothing to implement
          break;
      }

      REX_ERROR("Could not convert \"shaderType\" of type: {0}", shaderType);
      return "Unsupported Shader Type";
    }
  } // namespace conversions
  namespace shader
  {
    //-----------------------------------------------------------------------
    rsl::string create_shader_compilation_error_message(u32 shaderType)
    {
      rsl::stringstream stream;

      stream << "Could not compile ";
      stream << conversions::to_string(shaderType).data();

      REX_TODO("Make sure we use an rsl::string here, this can only happen if rsl::stringstream is implemented properly.");
      return rsl::string(stream.str().c_str());
    }

    //-----------------------------------------------------------------------
    u32 create_shader(u32 shaderType, u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength)
    {
      const u32 shader = glCreateShader(static_cast<GLenum>(shaderType));

      glShaderSource(shader, rsl::safe_numeric_cast<GLsizei>(shaderElementCount), shaderElements, shaderElementLength);
      glCompileShader(shader);

      s32 success = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

      if(success == 0)
      {
        rsl::array<char, 512> info_log;
        glGetShaderInfoLog(shader, 512, nullptr, info_log.data());

        REX_ERROR(rsl::string(info_log.data()));
        REX_ERROR(create_shader_compilation_error_message(shaderType));

        return 0;
      }

      return shader;
    }

    //-----------------------------------------------------------------------
    void destroy_shader(u32 shader)
    {
      glDeleteShader(shader);
    }
  } // namespace shader

  //-----------------------------------------------------------------------
  u32 create_vertex_shaders(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength /*= nullptr*/)
  {
    return shader::create_shader(GL_VERTEX_SHADER, shaderElementCount, shaderElements, shaderElementLength);
  }
  //-----------------------------------------------------------------------
  u32 create_fragment_shaders(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength /*= nullptr*/)
  {
    return shader::create_shader(GL_FRAGMENT_SHADER, shaderElementCount, shaderElements, shaderElementLength);
  }

  //-----------------------------------------------------------------------
  void destroy_vertex_shader(u32 shader)
  {
    shader::destroy_shader(shader);
  }
  //-----------------------------------------------------------------------
  void destroy_fragment_shader(u32 shader)
  {
    shader::destroy_shader(shader);
  }

} // namespace rex

#undef REX_ENABLE_STD_ALIAS