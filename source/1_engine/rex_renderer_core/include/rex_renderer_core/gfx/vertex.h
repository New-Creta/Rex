#pragma once

#include "rex_renderer_core/resources/input_layout.h"

#ifdef REX_COMPILER_MSVC
  #pragma warning(push)
  #pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#endif

#include <glm/glm.hpp>

#ifdef REX_COMPILER_MSVC
  #pragma warning(pop)
#endif

// This header describes a list of vertex formats supported by the engine.

namespace rex
{
  namespace gfx
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
      VertexPos();
      explicit VertexPos(const glm::vec3& position);

      static InputLayoutDesc layout();

    public:
      glm::vec3 position;
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
      VertexPosCol();
      VertexPosCol(const glm::vec3& position, const glm::vec4& color);

      static InputLayoutDesc layout();

    public:
      glm::vec3 position;
      glm::vec4 color;
    };

    bool operator==(const VertexPosCol& v1, const VertexPosCol& v2);
    bool operator!=(const VertexPosCol& v1, const VertexPosCol& v2);

    //-----------------------------------------------------------------------
    // VERTEX POS COL UV
    //
    // ATTRIBUTES:
    //  - POSITION
    //  - COLOR
    //  - UV
    //-----------------------------------------------------------------------
    class VertexPosColUv
    {
    public:
      VertexPosColUv();
      VertexPosColUv(const glm::vec3& position, const glm::vec4& color, const glm::vec2& uv);

      static InputLayoutDesc layout();

    public:
      glm::vec3 position;
      glm::vec4 color;
      glm::vec2 uv;
    };

    bool operator==(const VertexPosCol& v1, const VertexPosCol& v2);
    bool operator!=(const VertexPosCol& v1, const VertexPosCol& v2);

    //-----------------------------------------------------------------------
    // VERTEX POS COL
    //
    // ATTRIBUTES:
    //  - POSITION
    //  - NORMAL
    //  - COLOR
    //-----------------------------------------------------------------------
    class VertexPosNormCol
    {
    public:
      VertexPosNormCol();
      VertexPosNormCol(const glm::vec3& position, const glm::vec3& norm, const glm::vec4& color);

      static InputLayoutDesc layout();

    public:
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec4 color;
    };

    bool operator==(const VertexPosNormCol& v1, const VertexPosNormCol& v2);
    bool operator!=(const VertexPosNormCol& v1, const VertexPosNormCol& v2);

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
      VertexPosTex();
      VertexPosTex(const glm::vec3& position, const glm::vec2& uv);

      static InputLayoutDesc layout();

    public:
      glm::vec3 position;
      glm::vec2 uv;
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
      VertexPosColTex();
      VertexPosColTex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& uv);

      static InputLayoutDesc layout();

    public:
      glm::vec3 position;
      glm::vec4 color;
      glm::vec2 uv;
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
      VertexPosNormTex();
      VertexPosNormTex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv);

      static InputLayoutDesc layout();

    public:
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 uv;
    };

    bool operator==(const VertexPosNormTex& v1, const VertexPosNormTex& v2);
    bool operator!=(const VertexPosNormTex& v1, const VertexPosNormTex& v2);
  } // namespace gfx
} // namespace rex
