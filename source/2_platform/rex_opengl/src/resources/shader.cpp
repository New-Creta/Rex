#include "rex_opengl/resources/shader.h"

#include "rex_engine/diagnostics/logging.h"

#define REX_ENABLE_STD_ALIAS
#include "rex_std/sstream.h"

#if REX_PLATFORM_X64
#include <glad/gl.h>
#else
#error "Unsupported platform"
#endif

// TODO:  sstream define REX_ENABLE_STD_ALIAS(stringstream) is not working properly
//        adding workaround here
namespace rsl
{
  inline namespace v1
  {
    using stringstream = std::stringstream;
  }
} // namespace rsl

namespace rex
{
    namespace conversions
    {
        //-----------------------------------------------------------------------
        char8* to_string(u32 shaderType)
        {
            switch (shaderType)
            {
            case GL_VERTEX_SHADER:
                return "Vertex Shader";
            case GL_FRAGMENT_SHADER:
                return "Fragment Shader";
            }

            REX_ERROR("Could not convert \"shaderType\" of type: {0}", shaderType);
            return "Unsupported Shader Type";
        }
    }
    namespace shader
    {
        //-----------------------------------------------------------------------
        rsl::string create_shader_compilation_error_message(u32 shaderType)
        {
            rsl::stringstream stream;

            stream << "Could not compile ";
            stream << conversions::to_string(shaderType);

            REX_TODO("Make sure we use an rsl::string here, this can only happen if rsl::stringstream is implemented properly.");
            return rsl::string(stream.str().c_str());
        }

        //-----------------------------------------------------------------------
        u32 create_shader(u32 shaderType, u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength)
        {
            u32 shader = glCreateShader((GLenum)shaderType);

            glShaderSource(shader, (GLsizei)shaderElementCount, shaderElements, shaderElementLength);
            glCompileShader(shader);
            
            s32  success;
            char infoLog[512];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
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
    }

    //-----------------------------------------------------------------------
    u32 create_vertex_shader(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength /*= nullptr*/)
    {
        return shader::create_shader(GL_VERTEX_SHADER, shaderElementCount, shaderElements, shaderElementLength);
    }
    //-----------------------------------------------------------------------
    u32 create_fragment_shader(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength /*= nullptr*/)
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

}

#undef REX_ENABLE_STD_ALIAS