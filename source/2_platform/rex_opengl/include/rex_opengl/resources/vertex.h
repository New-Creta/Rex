#pragma once

#include "rex_opengl/resources/buffer_layout.h"

#include "rex_std/unordered_map.h"

#include <glm/glm.hpp>

namespace rex
{
    //-----------------------------------------------------------------------
    // VERTEX POS
    // 
    // ATTRIBUTES:
    //  - POSITION
    //-----------------------------------------------------------------------
    class VertexPos
    {
    public:
        static BufferLayout get_layout();

        VertexPos();
        VertexPos(const glm::vec3& position);

        const glm::vec3& get_position() const;

    private:
        glm::vec3 m_position;
    };

    bool operator==(const VertexPos& v1, const VertexPos& v2);
    bool operator!=(const VertexPos& v1, const VertexPos& v2);

    //-----------------------------------------------------------------------
    // VERTEX POS COL
    // 
    // ATTRIBUTES:
    //  - POSITION
    //  - COLOR
    //-----------------------------------------------------------------------
    class VertexPosCol
    {
    public:
        static BufferLayout get_layout();

        VertexPosCol();
        VertexPosCol(const glm::vec3& position, const glm::vec3& color);

        const glm::vec3& get_position() const;
        const glm::vec3& get_color() const;

    private:
        glm::vec3 m_position;
        glm::vec3 m_color;
    };

    bool operator==(const VertexPosCol& v1, const VertexPosCol& v2);
    bool operator!=(const VertexPosCol& v1, const VertexPosCol& v2);

    //-----------------------------------------------------------------------
    // VERTEX POS TEX
    // 
    // ATTRIBUTES:
    //  - POSITION
    //  - UV
    //-----------------------------------------------------------------------
    class VertexPosTex
    {
    public:
        static BufferLayout get_layout();

        VertexPosTex();
        VertexPosTex(const glm::vec3& position, const glm::vec2& uv);

        const glm::vec3& get_position() const;
        const glm::vec2& get_uv() const;

    private:
        glm::vec3 m_position;
        glm::vec2 m_uv;
    };

    bool operator==(const VertexPosTex& v1, const VertexPosTex& v2);
    bool operator!=(const VertexPosTex& v1, const VertexPosTex& v2);

    //-----------------------------------------------------------------------
    // VERTEX POS COL TEX
    // 
    // ATTRIBUTES:
    //  - POSITION
    //  - COLOR
    //  - UV
    //-----------------------------------------------------------------------
    class VertexPosColTex
    {
    public:
        static BufferLayout get_layout();

        VertexPosColTex();
        VertexPosColTex(const glm::vec3& position, const glm::vec3& color, const glm::vec2& uv);

        const glm::vec3& get_position() const;
        const glm::vec3& get_color() const;
        const glm::vec2& get_uv() const;

    private:
        glm::vec3 m_position;
        glm::vec3 m_color;
        glm::vec2 m_uv;
    };

    bool operator==(const VertexPosColTex& v1, const VertexPosColTex& v2);
    bool operator!=(const VertexPosColTex& v1, const VertexPosColTex& v2);

    //-----------------------------------------------------------------------
    // All available vertex layouts
    const rsl::unordered_map<u64, BufferLayout>& get_buffer_layouts();
}