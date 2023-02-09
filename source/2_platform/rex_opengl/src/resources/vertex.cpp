#include "resources/vertex.h"

namespace rex
{
    //-----------------------------------------------------------------------
    // VERTEX POS
    // 
    // ATTRIBUTES:
    //  - POSITION
    //-----------------------------------------------------------------------
    BufferLayout VertexPos::get_layout()
    {
        BufferLayoutBuilder builder;
        builder.add_buffer_element(DataType::Value::VEC3);
        return builder.build();
    }

    //-----------------------------------------------------------------------
    VertexPos& VertexPos::operator=(const VertexPos& other)
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = other.m_position;

        return *this;
    }

    //-----------------------------------------------------------------------
    VertexPos& VertexPos::operator=(VertexPos&& other) noexcept
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = rsl::exchange(other.m_position, {});

        return *this;
    }

    //-----------------------------------------------------------------------
    VertexPos::VertexPos()
        :m_position(0.0f)
    {

    }
    //-----------------------------------------------------------------------
    VertexPos::VertexPos(const VertexPos& other)
        :m_position(other.m_position)
    {

    }
    //-----------------------------------------------------------------------
    VertexPos::VertexPos(VertexPos&& other) noexcept
        :m_position(rsl::exchange(other.m_position, {}))
    {

    }

    //-----------------------------------------------------------------------
    VertexPos::VertexPos(const glm::vec3& position)
        :m_position(position)
    {

    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPos::get_position() const
    {
        return m_position;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPos& v1, const VertexPos& v2)
    {
        return v1.get_position().x == v2.get_position().x 
            && v1.get_position().y == v2.get_position().y
            && v1.get_position().z == v2.get_position().z;
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPos& v1, const VertexPos& v2)
    {
        return !(v1 == v2);
    }

    //-----------------------------------------------------------------------
    // VERTEX POS COL
    // 
    // ATTRIBUTES:
    //  - POSITION
    //  - COLOR
    //-----------------------------------------------------------------------
    BufferLayout VertexPosCol::get_layout()
    {
        BufferLayoutBuilder builder;
        builder.add_buffer_element(DataType::Value::VEC3);
        builder.add_buffer_element(DataType::Value::VEC3);
        return builder.build();
    }

    //-----------------------------------------------------------------------
    VertexPosCol& VertexPosCol::operator=(const VertexPosCol& other)
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = other.m_position;
        this->m_color = other.m_color;

        return *this;
    }
    //-----------------------------------------------------------------------
    VertexPosCol& VertexPosCol::operator=(VertexPosCol&& other) noexcept
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = rsl::exchange(other.m_position, {});
        this->m_color = rsl::exchange(other.m_color, {});

        return *this;
    }

    //-----------------------------------------------------------------------
    VertexPosCol::VertexPosCol()
        :m_position(0.0f)
        , m_color(0.0f)
    {

    }
    //-----------------------------------------------------------------------
    VertexPosCol::VertexPosCol(const VertexPosCol& other)
        :m_position(other.m_position)
        , m_color(other.m_color)
    {

    }
    //-----------------------------------------------------------------------
    VertexPosCol::VertexPosCol(VertexPosCol&& other) noexcept
        :m_position(rsl::exchange(other.m_position, {}))
        , m_color(rsl::exchange(other.m_color, {}))
    {

    }

    //-----------------------------------------------------------------------
    VertexPosCol::VertexPosCol(const glm::vec3& position, const glm::vec3& color)
        :m_position(position)
        , m_color(color)
    {

    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosCol::get_position() const
    {
        return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosCol::get_color() const
    {
        return m_color;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosCol& v1, const VertexPosCol& v2)
    {
        return v1.get_position().x == v2.get_position().x
            && v1.get_position().y == v2.get_position().y
            && v1.get_position().z == v2.get_position().z
            && v1.get_color().x == v2.get_color().x
            && v1.get_color().y == v2.get_color().y
            && v1.get_color().z == v2.get_color().z;
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosCol& v1, const VertexPosCol& v2)
    {
        return !(v1 == v2);
    }

    //-----------------------------------------------------------------------
    // VERTEX POS TEX
    // 
    // ATTRIBUTES:
    //  - POSITION
    //  - UV
    //-----------------------------------------------------------------------
    BufferLayout VertexPosTex::get_layout()
    {
        BufferLayoutBuilder builder;
        builder.add_buffer_element(DataType::Value::VEC3);
        builder.add_buffer_element(DataType::Value::VEC2);
        return builder.build();
    }

    //-----------------------------------------------------------------------
    VertexPosTex& VertexPosTex::operator=(const VertexPosTex& other)
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = other.m_position;
        this->m_uv = other.m_uv;

        return *this;
    }
    //-----------------------------------------------------------------------
    VertexPosTex& VertexPosTex::operator=(VertexPosTex&& other) noexcept
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = rsl::exchange(other.m_position, {});
        this->m_uv = rsl::exchange(other.m_uv, {});

        return *this;
    }

    //-----------------------------------------------------------------------
    VertexPosTex::VertexPosTex()
        :m_position(0.0f)
        ,m_uv(0.0f)
    {

    }
    //-----------------------------------------------------------------------
    VertexPosTex::VertexPosTex(const VertexPosTex& other)
        :m_position(other.m_position)
        ,m_uv(other.m_uv)
    {

    }
    //-----------------------------------------------------------------------
    VertexPosTex::VertexPosTex(VertexPosTex&& other) noexcept
        :m_position(rsl::exchange(other.m_position, {}))
        , m_uv(rsl::exchange(other.m_uv, {}))
    {

    }

    //-----------------------------------------------------------------------
    VertexPosTex::VertexPosTex(const glm::vec3& position, const glm::vec2& uv)
        :m_position(position)
        , m_uv(uv)
    {

    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosTex::get_position() const
    {
        return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec2& VertexPosTex::get_uv() const
    {
        return m_uv;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosTex& v1, const VertexPosTex& v2)
    {
        return v1.get_position().x == v2.get_position().x
            && v1.get_position().y == v2.get_position().y
            && v1.get_position().z == v2.get_position().z
            && v1.get_uv().x == v2.get_uv().x
            && v1.get_uv().y == v2.get_uv().y;
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosTex& v1, const VertexPosTex& v2)
    {
        return !(v1 == v2);
    }

    //-----------------------------------------------------------------------
    // VERTEX POS COL TEX
    // 
    // ATTRIBUTES:
    //  - POSITION
    //  - COLOR
    //  - UV
    //-----------------------------------------------------------------------
    BufferLayout VertexPosColTex::get_layout()
    {
        BufferLayoutBuilder builder;
        builder.add_buffer_element(DataType::Value::VEC3);
        builder.add_buffer_element(DataType::Value::VEC3);
        builder.add_buffer_element(DataType::Value::VEC2);
        return builder.build();
    }

    //-----------------------------------------------------------------------
    VertexPosColTex& VertexPosColTex::operator=(const VertexPosColTex& other)
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = other.m_position;
        this->m_color = other.m_color;
        this->m_uv = other.m_uv;

        return *this;
    }
    //-----------------------------------------------------------------------
    VertexPosColTex& VertexPosColTex::operator=(VertexPosColTex&& other) noexcept
    {
        if (*this == other)
        {
            return *this;
        }

        this->m_position = rsl::exchange(other.m_position, {});
        this->m_color = rsl::exchange(other.m_color, {});
        this->m_uv = rsl::exchange(other.m_uv, {});

        return *this;
    }

    //-----------------------------------------------------------------------
    VertexPosColTex::VertexPosColTex()
        :m_position(0.0f)
        , m_color(0.0f)
        , m_uv(0.0f)
    {

    }
    //-----------------------------------------------------------------------
    VertexPosColTex::VertexPosColTex(const VertexPosColTex& other)
        :m_position(other.m_position)
        , m_color(other.m_color)
        , m_uv(other.m_uv)
    {

    }
    //-----------------------------------------------------------------------
    VertexPosColTex::VertexPosColTex(VertexPosColTex&& other) noexcept
        :m_position(rsl::exchange(other.m_position, {}))
        , m_color(rsl::exchange(other.m_color, {}))
        , m_uv(rsl::exchange(other.m_uv, {}))
    {

    }

    //-----------------------------------------------------------------------
    VertexPosColTex::VertexPosColTex(const glm::vec3& position, const glm::vec3& color, const glm::vec2& uv)
        :m_position(position)
        , m_color(color)
        , m_uv(uv)
    {

    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosColTex::get_position() const
    {
        return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosColTex::get_color() const
    {
        return m_color;
    }
    //-------------------------------------------------------------------------
    const glm::vec2& VertexPosColTex::get_uv() const
    {
        return m_uv;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosColTex& v1, const VertexPosColTex& v2)
    {
        return v1.get_position().x == v2.get_position().x
            && v1.get_position().y == v2.get_position().y
            && v1.get_position().z == v2.get_position().z
            && v1.get_color().x == v2.get_color().x
            && v1.get_color().y == v2.get_color().y
            && v1.get_color().z == v2.get_color().z
            && v1.get_uv().x == v2.get_uv().x
            && v1.get_uv().y == v2.get_uv().y;
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosColTex& v1, const VertexPosColTex& v2)
    {
        return !(v1 == v2);
    }

    //-----------------------------------------------------------------------
    rsl::unordered_map<u64, BufferLayout>& get_buffer_layouts()
    {
        static rsl::unordered_map<u64, BufferLayout> buffer_layouts =
        {
            {typeid(VertexPos).hash_code(), VertexPos::get_layout()},
            {typeid(VertexPosCol).hash_code(), VertexPosCol::get_layout()},
            {typeid(VertexPosTex).hash_code(), VertexPosTex::get_layout()},
            {typeid(VertexPosColTex).hash_code(), VertexPosColTex::get_layout()},
        };

        return buffer_layouts;
    }
}