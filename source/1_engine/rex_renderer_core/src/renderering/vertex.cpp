#include "rex_renderer_core/rendering/vertex.h"

#include "rex_std/bonus/utility/type_id.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)

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
    const glm::vec3& VertexPos::position() const
    {
      return m_position;
    }

    //-----------------------------------------------------------------------
    rhi::InputLayoutDesc VertexPos::layout()
    {
      rex::rhi::InputLayoutDesc desc;
      desc.input_layout =
      {
        rex::rhi::InputLayoutElementDesc
        {
          "POSITION",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPos& v1, const VertexPos& v2)
    {
      // clang-format off
            return v1.position().x == v2.position().x && v1.position().y == v2.position().y && v1.position().z == v2.position().z;
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
        : m_position(0.0f)
        , m_color(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosCol::VertexPosCol(const glm::vec3& position, const glm::vec4& color)
        : m_position(position)
        , m_color(color)
    {
    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosCol::position() const
    {
      return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec4& VertexPosCol::color() const
    {
      return m_color;
    }
    //-----------------------------------------------------------------------
    rhi::InputLayoutDesc VertexPosCol::layout()
    {
      rex::rhi::InputLayoutDesc desc;
      desc.input_layout =
      {
        rex::rhi::InputLayoutElementDesc
        {
          "POSITION",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "COLOR", rex::renderer::VertexBufferFormat::Float4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 12, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosCol& v1, const VertexPosCol& v2)
    {
      // clang-format off
            return v1.position().x == v2.position().x && v1.position().y == v2.position().y && v1.position().z == v2.position().z && v1.color().x == v2.color().x && v1.color().y == v2.color().y && v1.color().z == v2.color().z && v1.color().a == v2.color().a;
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
        : m_position(0.0f)
        , m_normal(0.0f)
        , m_color(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosNormCol::VertexPosNormCol(const glm::vec3& position, const glm::vec3& normal, const glm::vec4& color)
        : m_position(position)
        , m_normal(normal)
        , m_color(color)
    {
    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosNormCol::position() const
    {
      return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosNormCol::normal() const
    {
      return m_normal;
    }
    //-----------------------------------------------------------------------
    const glm::vec4& VertexPosNormCol::color() const
    {
      return m_color;
    }
    //-----------------------------------------------------------------------
    rhi::InputLayoutDesc VertexPosNormCol::layout()
    {
      rex::rhi::InputLayoutDesc desc;
      desc.input_layout =
      {
        rex::rhi::InputLayoutElementDesc
        {
          "POSITION",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "NORMAL",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 12, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "COLOR", rex::renderer::VertexBufferFormat::Float4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 24, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosNormCol& v1, const VertexPosNormCol& v2)
    {
      // clang-format off
      return v1.position() == v2.position() && v1.normal() == v2.normal() && v1.color() == v2.color();
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
    const glm::vec3& VertexPosTex::position() const
    {
      return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec2& VertexPosTex::uv() const
    {
      return m_uv;
    }
    //-----------------------------------------------------------------------
    rhi::InputLayoutDesc VertexPosTex::layout()
    {
      rex::rhi::InputLayoutDesc desc;
      desc.input_layout =
      {
        rex::rhi::InputLayoutElementDesc
        {
          "POSITION",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "TEXCOORD",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 12, 0
        },
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosTex& v1, const VertexPosTex& v2)
    {
      // clang-format off
            return v1.position().x == v2.position().x && v1.position().y == v2.position().y && v1.position().z == v2.position().z && v1.uv().x == v2.uv().x && v1.uv().y == v2.uv().y;
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
        : m_position(0.0f)
        , m_color(0.0f, 0.0f, 0.0f, 1.0f)
        , m_uv(0.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosColTex::VertexPosColTex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& uv)
        : m_position(position)
        , m_color(color)
        , m_uv(uv)
    {
    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosColTex::position() const
    {
      return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec4& VertexPosColTex::color() const
    {
      return m_color;
    }
    //-------------------------------------------------------------------------
    const glm::vec2& VertexPosColTex::uv() const
    {
      return m_uv;
    }
    //-----------------------------------------------------------------------
    rhi::InputLayoutDesc VertexPosColTex::layout()
    {
      rex::rhi::InputLayoutDesc desc;
      desc.input_layout =
      {
        rex::rhi::InputLayoutElementDesc
        {
          "POSITION",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "COLOR",  rex::renderer::VertexBufferFormat::Float4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 12, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "TEXCOORD",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 28, 0
        },
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosColTex& v1, const VertexPosColTex& v2)
    {
      // clang-format off
            return v1.position().x == v2.position().x && v1.position().y == v2.position().y && v1.position().z == v2.position().z && v1.color().x == v2.color().x && v1.color().y == v2.color().y && v1.color().z == v2.color().z && v1.color().a == v2.color().a && v1.uv().x == v2.uv().x && v1.uv().y == v2.uv().y;
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
        : m_position(0.0f)
        , m_normal(0.0f)
        , m_uv(0.0f)
    {
    }

    //-----------------------------------------------------------------------
    VertexPosNormTex::VertexPosNormTex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv)
        : m_position(position)
        , m_normal(normal)
        , m_uv(uv)
    {
    }

    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosNormTex::position() const
    {
      return m_position;
    }
    //-----------------------------------------------------------------------
    const glm::vec3& VertexPosNormTex::normal() const
    {
      return m_normal;
    }
    //-------------------------------------------------------------------------
    const glm::vec2& VertexPosNormTex::uv() const
    {
      return m_uv;
    }
    //-----------------------------------------------------------------------
    rhi::InputLayoutDesc VertexPosNormTex::layout()
    {
      rex::rhi::InputLayoutDesc desc;
      desc.input_layout =
      {
        rex::rhi::InputLayoutElementDesc
        {
          "POSITION",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "NORMAL",  rex::renderer::VertexBufferFormat::Float3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 12, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "TEXCOORD", rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 24, 0
        }
      };

      return desc;
    }

    //-----------------------------------------------------------------------
    bool operator==(const VertexPosNormTex& v1, const VertexPosNormTex& v2)
    {
      // clang-format off
            return v1.position().x == v2.position().x && v1.position().y == v2.position().y && v1.position().z == v2.position().z && v1.normal().x == v2.normal().x && v1.normal().y == v2.normal().y && v1.normal().z == v2.normal().z && v1.uv().x == v2.uv().x && v1.uv().y == v2.uv().y;
      // clang-format on
    }
    //-----------------------------------------------------------------------
    bool operator!=(const VertexPosNormTex& v1, const VertexPosNormTex& v2)
    {
      return !(v1 == v2);
    }
  } // namespace renderer
} // namespace rex

// NOLINTEND(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)