#pragma once

#include "rex_renderer_resources/resources/buffer_layout.h"
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
  namespace renderer
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
      static BufferLayout layout();

      VertexPos();
      explicit VertexPos(const glm::vec3& position);

      const glm::vec3& position() const;

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
      static BufferLayout layout();

      VertexPosCol();
      VertexPosCol(const glm::vec3& position, const glm::vec4& color);

      const glm::vec3& position() const;
      const glm::vec4& color() const;

    private:
      glm::vec3 m_position;
      glm::vec4 m_color;
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
      static BufferLayout layout();

      VertexPosTex();
      VertexPosTex(const glm::vec3& position, const glm::vec2& uv);

      const glm::vec3& position() const;
      const glm::vec2& uv() const;

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
      static BufferLayout layout();

      VertexPosColTex();
      VertexPosColTex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& uv);

      const glm::vec3& position() const;
      const glm::vec4& color() const;
      const glm::vec2& uv() const;

    private:
      glm::vec3 m_position;
      glm::vec4 m_color;
      glm::vec2 m_uv;
    };

    bool operator==(const VertexPosColTex& v1, const VertexPosColTex& v2);
    bool operator!=(const VertexPosColTex& v1, const VertexPosColTex& v2);

    //-----------------------------------------------------------------------
    // VERTEX POS NORM TEX
    //
    // ATTRIBUTES:
    //  - POSITION
    //  - NORMAL
    //  - UV
    //-----------------------------------------------------------------------
    class VertexPosNormTex
    {
    public:
      static BufferLayout layout();

      VertexPosNormTex();
      VertexPosNormTex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv);

      const glm::vec3& position() const;
      const glm::vec3& normal() const;
      const glm::vec2& uv() const;

    private:
      glm::vec3 m_position;
      glm::vec3 m_normal;
      glm::vec2 m_uv;
    };

    bool operator==(const VertexPosNormTex& v1, const VertexPosNormTex& v2);
    bool operator!=(const VertexPosNormTex& v1, const VertexPosNormTex& v2);

    //-----------------------------------------------------------------------
    // All available vertex layouts
    const rsl::unordered_map<u64, BufferLayout>& buffer_layouts();
  } // namespace renderer
} // namespace rex
