#pragma once

#include "resources/resource.h"

#include <glm/glm.hpp>

namespace rex
{
    class ShaderUniformLocation
    {
    public:
        static ShaderUniformLocation EMPTY;

        //-------------------------------------------------------------------------
        ShaderUniformLocation()
            :m_location(-1)
        {}
        //-------------------------------------------------------------------------
        ShaderUniformLocation(s32 loc)
            :m_location(loc)
        {}

        //-------------------------------------------------------------------------
        operator bool() const
        {
            return m_location != -1;
        }
        //-------------------------------------------------------------------------
        operator s32() const
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

    using UniformLocationMap = rsl::unordered_map<size_t, ShaderUniformLocation>;

    class ShaderProgram : public Resource
    {
    public:
        ShaderProgram();
        ShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode);
        ~ShaderProgram() override;

        u32 get_resource_id() const override;

        void bind() const override;
        void unbind() const override;

        void release();

        bool set_uniform(const rsl::string& name, const glm::mat3& mat);
        bool set_uniform(const rsl::string& name, const glm::mat4& mat);

        bool set_uniform(const rsl::string& name, const float value);
        bool set_uniform(const rsl::string& name, const glm::vec2& value);
        bool set_uniform(const rsl::string& name, const glm::vec3& value);
        bool set_uniform(const rsl::string& name, const glm::vec4& value);

        bool set_uniform(const rsl::string& name, const glm::ivec2& value);
        bool set_uniform(const rsl::string& name, const glm::ivec3& value);
        bool set_uniform(const rsl::string& name, const glm::ivec4& value);

        bool set_uniform(const rsl::string& name, const float* values, u32 size);

        bool set_uniform(const rsl::string& name, const s32 value);
        bool set_uniform(const rsl::string& name, const u32 value);
        bool set_uniform(const rsl::string& name, const bool value);

    private:
        u32 m_resource_id;
        UniformLocationMap m_uniform_locations;
        mutable bool m_bound;
    };
}