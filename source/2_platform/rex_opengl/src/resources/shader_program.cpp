#include "resources/shader_program.h"
#include "resources/shader.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#if defined _MSC_VER
#   pragma warning (push)
#   pragma warning (disable : 4018)
#   pragma warning (disable : 4100)
#   pragma warning (disable : 4127)
#   pragma warning (disable : 4189)
#   pragma warning (disable : 4201)
#   pragma warning (disable : 4244)
#   pragma warning (disable : 4245)
#   pragma warning (disable : 4267)
#   pragma warning (disable : 4389)
#   pragma warning (disable : 4456)
#   pragma warning (disable : 4457)
#   pragma warning (disable : 4458)
#   pragma warning (disable : 4459)
#   pragma warning (disable : 4701)
#   pragma warning (disable : 4996)
#endif

#include <glm/gtc/type_ptr.hpp>

#if defined _MSC_VER
#   pragma warning (pop)
#endif


namespace rex
{
    namespace shader_program
    {
        //-----------------------------------------------------------------------
        rsl::string create_shader_program_linking_error_message()
        {
            rsl::stringstream stream;

            stream << "Could not link shader program";

            return stream.str();
        }

        //-------------------------------------------------------------------------
        bool find_and_cache_uniform(const u32& programID, const rsl::string& inName, rsl::unordered_map<size_t, ShaderUniformLocation>& uniformLocations)
        {
            size_t hash = rsl::hash<rsl::string>{}(inName);

            if (uniformLocations.find(hash) == rsl::end(uniformLocations))
            {
                REX_TRACE("Caching uniform: {0}, for shader program with ID: {1}", inName, programID);

                s32 location = glGetUniformLocation(programID, inName.c_str());
                uniformLocations[hash] = { location };

                if (location == -1)
                {
                    REX_WARN("Uniform with name: \"{0}\" was not found", inName);
                    return false;
                }
            }

            return true;
        }
    }

    //-----------------------------------------------------------------------
    ShaderProgram::ShaderProgram()
        :m_resource_id(0)
        ,m_bound(false)
    {

    }
    //-----------------------------------------------------------------------
    ShaderProgram::ShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode)
        :m_resource_id(0)
        ,m_bound(false)
    {
        u32 vertex_shader_id = create_vertex_shader(1, &vertexShaderCode);
        u32 fragment_shader_id = create_fragment_shader(1, &fragmentShaderCode);

        m_resource_id = glCreateProgram();

        glAttachShader(m_resource_id, vertex_shader_id);
        glAttachShader(m_resource_id, fragment_shader_id);

        glLinkProgram(m_resource_id);

        s32  success;
        char infoLog[512];
        glGetProgramiv(m_resource_id, GL_LINK_STATUS, &success);

        if (!success) 
        {
            // Don't forget to delete the shader objects once we've linked (or failed to link) them into the program object; we no longer need them anymore
            glDeleteShader(vertex_shader_id);
            glDeleteShader(fragment_shader_id);

            glGetProgramInfoLog(m_resource_id, 512, NULL, infoLog);
            REX_ERROR(shader_program::create_shader_program_linking_error_message());

            m_resource_id = 0;

            return;
        }

        // Don't forget to delete the shader objects once we've linked (or failed to link) them into the program object; we no longer need them anymore
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

    //-----------------------------------------------------------------------
    ShaderProgram::~ShaderProgram()
    {
        release();
    }

    //-----------------------------------------------------------------------
    u32 ShaderProgram::get_resource_id() const
    {
        return m_resource_id;
    }

    //-----------------------------------------------------------------------
    void ShaderProgram::bind() const
    {
        glUseProgram(m_resource_id);

        m_bound = true;
    }
    //-----------------------------------------------------------------------
    void ShaderProgram::unbind() const
    {
        glUseProgram(0);

        m_bound = false;
    }

    //-----------------------------------------------------------------------
    void ShaderProgram::release()
    {
        glDeleteProgram(m_resource_id);

        m_resource_id = 0;
        m_bound = false;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::mat3& mat)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniformMatrix3fv(location, 1, false, glm::value_ptr(mat));

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::mat4& mat)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(mat));

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const float value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform1f(location, value);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::vec2& value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform2f(location, value.x, value.y);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::vec3& value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform3f(location, value.x, value.y, value.z);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::vec4& value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform4f(location, value.x, value.y, value.z, value.w);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::ivec2& value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform2i(location, value.x, value.y);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::ivec3& value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform3i(location, value.x, value.y, value.z);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const glm::ivec4& value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform4i(location, value.x, value.y, value.z, value.w);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const float* values, u32 size)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform1fv(location, size, values);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const s32 value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform1i(location, value);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const u32 value)
    {
        if (!shader_program::find_and_cache_uniform(m_resource_id, name, m_uniform_locations))
        {
            REX_WARN("Uniform ({0}) not found on shader.", name);
            return false;
        }

        s32 location = m_uniform_locations.at(rsl::hash<rsl::string>{}(name));
        glUniform1ui(location, value);

        return true;
    }

    //-----------------------------------------------------------------------
    bool ShaderProgram::set_uniform(const rsl::string& name, const bool value)
    {
        return set_uniform(name, value ? 1 : 0);
    }

}