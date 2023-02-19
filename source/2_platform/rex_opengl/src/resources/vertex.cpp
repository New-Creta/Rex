#include "rex_opengl/resources/vertex.h"

#include "rex_std_extra/utility/type_id.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)

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
    builder.add_buffer_element(DataType::Value::Vec3);
    return builder.build();
  }

  //-----------------------------------------------------------------------
  VertexPos::VertexPos()
      : m_position(0.0f)
  {
  }

  //-----------------------------------------------------------------------
  VertexPos::VertexPos(const glm::vec3& position)
      : m_position(position)
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
    return v1.get_position().x == v2.get_position().x && v1.get_position().y == v2.get_position().y && v1.get_position().z == v2.get_position().z;
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
    builder.add_buffer_element(DataType::Value::Vec3);
    builder.add_buffer_element(DataType::Value::Vec3);
    return builder.build();
  }

  //-----------------------------------------------------------------------
  VertexPosCol::VertexPosCol()
      : m_position(0.0f)
      , m_color(0.0f)
  {
  }

  //-----------------------------------------------------------------------
  VertexPosCol::VertexPosCol(const glm::vec3& position, const glm::vec3& color)
      : m_position(position)
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
    return v1.get_position().x == v2.get_position().x && v1.get_position().y == v2.get_position().y && v1.get_position().z == v2.get_position().z && v1.get_color().x == v2.get_color().x && v1.get_color().y == v2.get_color().y &&
           v1.get_color().z == v2.get_color().z;
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
    builder.add_buffer_element(DataType::Value::Vec3);
    builder.add_buffer_element(DataType::Value::Vec2);
    return builder.build();
  }

  //-----------------------------------------------------------------------
  VertexPosTex::VertexPosTex()
      : m_position(0.0f)
      , m_uv(0.0f)
  {
  }

  //-----------------------------------------------------------------------
  VertexPosTex::VertexPosTex(const glm::vec3& position, const glm::vec2& uv)
      : m_position(position)
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
    return v1.get_position().x == v2.get_position().x && v1.get_position().y == v2.get_position().y && v1.get_position().z == v2.get_position().z && v1.get_uv().x == v2.get_uv().x && v1.get_uv().y == v2.get_uv().y;
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
    builder.add_buffer_element(DataType::Value::Vec3);
    builder.add_buffer_element(DataType::Value::Vec3);
    builder.add_buffer_element(DataType::Value::Vec2);
    return builder.build();
  }

  //-----------------------------------------------------------------------
  VertexPosColTex::VertexPosColTex()
      : m_position(0.0f)
      , m_color(0.0f)
      , m_uv(0.0f)
  {
  }

  //-----------------------------------------------------------------------
  VertexPosColTex::VertexPosColTex(const glm::vec3& position, const glm::vec3& color, const glm::vec2& uv)
      : m_position(position)
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
    return v1.get_position().x == v2.get_position().x && v1.get_position().y == v2.get_position().y && v1.get_position().z == v2.get_position().z && v1.get_color().x == v2.get_color().x && v1.get_color().y == v2.get_color().y &&
           v1.get_color().z == v2.get_color().z && v1.get_uv().x == v2.get_uv().x && v1.get_uv().y == v2.get_uv().y;
  }
  //-----------------------------------------------------------------------
  bool operator!=(const VertexPosColTex& v1, const VertexPosColTex& v2)
  {
    return !(v1 == v2);
  }

  //-----------------------------------------------------------------------
  const rsl::unordered_map<u64, BufferLayout>& get_buffer_layouts()
  {
    static const rsl::unordered_map<u64, BufferLayout> s_buffer_layouts = {
        {rsl::type_id<VertexPos>().hash_code(), VertexPos::get_layout()},
        {rsl::type_id<VertexPosCol>().hash_code(), VertexPosCol::get_layout()},
        {rsl::type_id<VertexPosTex>().hash_code(), VertexPosTex::get_layout()},
        {rsl::type_id<VertexPosColTex>().hash_code(), VertexPosColTex::get_layout()},
    };

    return s_buffer_layouts;
  }
} // namespace rex

// NOLINTEND(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)