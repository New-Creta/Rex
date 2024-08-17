#include "rex_renderer_core/gfx/vertex.h"

#include "rex_std/bonus/utility/type_id.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)

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
    VertexPos::VertexPos()
        : position (0.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPos::VertexPos(const glm::vec3& position)
        : position (position)
    {
    }

    //-----------------------------------------------------------------------
    InputLayoutDesc VertexPos::layout()
    {
      InputLayoutDesc desc =
      {
        InputLayoutElementDesc
        {
          ShaderSemantic::Position,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 0, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPos& v1, const VertexPos& v2)
    {
      // clang-format off
            return v1.position == v2.position;
      // clang-format on
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
    VertexPosCol::VertexPosCol()
        : position (0.0f)
        , color(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosCol::VertexPosCol(const glm::vec3& position, const glm::vec4& color)
        : position (position)
        , color(color)
    {
    }

    //-----------------------------------------------------------------------
    InputLayoutDesc VertexPosCol::layout()
    {
      InputLayoutDesc desc
      {
        InputLayoutElementDesc
        {
          ShaderSemantic::Position,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 0, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::Color, VertexBufferFormat::Float4, InputLayoutClassification::PerVertex, 0, 0, 12, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosCol& v1, const VertexPosCol& v2)
    {
      // clang-format off
            return v1.position == v2.position && v1.color == v2.color;
      // clang-format on
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosCol& v1, const VertexPosCol& v2)
    {
      return !(v1 == v2);
    }

    //-----------------------------------------------------------------------
    // VERTEX POS COL
    //
    // ATTRIBUTES:
    //  - POSITION
    //  - NORMAL
    //  - COLOR
    //-----------------------------------------------------------------------
    VertexPosNormCol::VertexPosNormCol()
        : position (0.0f)
        , normal(0.0f)
        , color(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosNormCol::VertexPosNormCol(const glm::vec3& position, const glm::vec3& normal, const glm::vec4& color)
        : position (position)
        , normal(normal)
        , color(color)
    {
    }

    //-----------------------------------------------------------------------
    InputLayoutDesc VertexPosNormCol::layout()
    {
      InputLayoutDesc desc
      {
        InputLayoutElementDesc
        {
          ShaderSemantic::Position,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 0, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::Normal,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 12, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::Color, VertexBufferFormat::Float4, InputLayoutClassification::PerVertex, 0, 0, 24, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosNormCol& v1, const VertexPosNormCol& v2)
    {
      // clang-format off
      return v1.position == v2.position && v1.normal == v2.normal && v1.color == v2.color;
      // clang-format on
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosNormCol& v1, const VertexPosNormCol& v2)
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
    VertexPosTex::VertexPosTex()
        : position (0.0f)
        , uv(0.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosTex::VertexPosTex(const glm::vec3& position, const glm::vec2& uv)
        : position (position)
        , uv(uv)
    {
    }

    //-----------------------------------------------------------------------
    InputLayoutDesc VertexPosTex::layout()
    {
      InputLayoutDesc desc
      {
        InputLayoutElementDesc
        {
          ShaderSemantic::Position,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 0, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::TexCoord,  VertexBufferFormat::Float2, InputLayoutClassification::PerVertex, 0, 0, 12, 0
        },
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosTex& v1, const VertexPosTex& v2)
    {
      // clang-format off
            return v1.position == v2.position && v1.uv == v2.uv;
      // clang-format on
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
    VertexPosColTex::VertexPosColTex()
        : position (0.0f)
        , color(0.0f, 0.0f, 0.0f, 1.0f)
        , uv(0.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosColTex::VertexPosColTex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& uv)
        : position (position)
        , color(color)
        , uv(uv)
    {
    }

    //-----------------------------------------------------------------------
    InputLayoutDesc VertexPosColTex::layout()
    {
      InputLayoutDesc desc
      {
        InputLayoutElementDesc
        {
          ShaderSemantic::Position,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 0, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::Color,  VertexBufferFormat::Float4, InputLayoutClassification::PerVertex, 0, 0, 12, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::TexCoord,  VertexBufferFormat::Float2, InputLayoutClassification::PerVertex, 0, 0, 28, 0
        },
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosColTex& v1, const VertexPosColTex& v2)
    {
      // clang-format off
            return v1.position == v2.position && v1.color == v2.color && v1.uv == v2.uv;
      // clang-format on
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosColTex& v1, const VertexPosColTex& v2)
    {
      return !(v1 == v2);
    }

    //-----------------------------------------------------------------------
    // VERTEX POS NORM TEX
    //
    // ATTRIBUTES:
    //  - POSITION
    //  - NORMAL
    //  - UV
    //-----------------------------------------------------------------------
    VertexPosNormTex::VertexPosNormTex()
        : position (0.0f)
        , normal(0.0f)
        , uv(0.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosNormTex::VertexPosNormTex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv)
        : position (position)
        , normal(normal)
        , uv(uv)
    {
    }

    //-----------------------------------------------------------------------
    InputLayoutDesc VertexPosNormTex::layout()
    {
      InputLayoutDesc desc
      {
        InputLayoutElementDesc
        {
          ShaderSemantic::Position,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 0, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::Normal,  VertexBufferFormat::Float3, InputLayoutClassification::PerVertex, 0, 0, 12, 0
        },
        InputLayoutElementDesc
        {
          ShaderSemantic::TexCoord, VertexBufferFormat::Float2, InputLayoutClassification::PerVertex, 0, 0, 24, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosNormTex& v1, const VertexPosNormTex& v2)
    {
      // clang-format off
            return v1.position == v2.position && v1.normal == v2.normal && v1.uv == v2.uv;
      // clang-format on
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosNormTex& v1, const VertexPosNormTex& v2)
    {
      return !(v1 == v2);
    }
  } // namespace gfx
} // namespace rex

// NOLINTEND(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)