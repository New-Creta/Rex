#pragma once

#include "resources/buffer_layout.h"

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
        VertexPos(const VertexPos& other);
        VertexPos(VertexPos&& other) noexcept;

        VertexPos& operator=(const VertexPos& other);
        VertexPos& operator=(VertexPos&& other) noexcept;

    public:
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
        VertexPosCol(const VertexPosCol& other);
        VertexPosCol(VertexPosCol&& other) noexcept;

        VertexPosCol& operator=(const VertexPosCol& other);
        VertexPosCol& operator=(VertexPosCol&& other) noexcept;

    public:
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
        VertexPosTex(const VertexPosTex& other);
        VertexPosTex(VertexPosTex&& other) noexcept;

        VertexPosTex& operator=(const VertexPosTex& other);
        VertexPosTex& operator=(VertexPosTex&& other) noexcept;

    public:
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
        VertexPosColTex(const VertexPosColTex& other);
        VertexPosColTex(VertexPosColTex&& other) noexcept;

        VertexPosColTex& operator=(const VertexPosColTex& other);
        VertexPosColTex& operator=(VertexPosColTex&& other) noexcept;

    public:
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
    rsl::unordered_map<u64, BufferLayout>& get_buffer_layouts();
}