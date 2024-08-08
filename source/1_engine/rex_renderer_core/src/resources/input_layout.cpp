#include "rex_renderer_core/resources/input_layout.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/engine/invalid_object.h"
#include "rex_engine/string/stringid.h"

namespace rex
{
	namespace gfx
	{
    rsl::string_view shader_semantic_name(ShaderSemantic semantic)
    {
      switch (semantic)
      {
      case rex::gfx::ShaderSemantic::Color:         return "COLOR";
      case rex::gfx::ShaderSemantic::Position:      return "POSITION";
      case rex::gfx::ShaderSemantic::PSize:         return "PSIZE";
      case rex::gfx::ShaderSemantic::BiNormal:      return "BINORMAL";
      case rex::gfx::ShaderSemantic::BlendIndices:  return "BLENDINDICES";
      case rex::gfx::ShaderSemantic::BlendWeight:   return "BLENDWEIGHT";
      case rex::gfx::ShaderSemantic::Normal:        return "NORMAL";
      case rex::gfx::ShaderSemantic::PositionT:     return "POSITIONT";
      case rex::gfx::ShaderSemantic::Tangent:       return "TANGENT";
      case rex::gfx::ShaderSemantic::TexCoord:      return "TEXCOORD";
      case rex::gfx::ShaderSemantic::Fog:           return "FOG";
      case rex::gfx::ShaderSemantic::TessFactor:    return "TESSFACTOR";
      }

      return "";
    }
    ShaderSemantic shader_semantic_type(rsl::string_view semantic)
    {
      rsl::hash_result semantic_hash = rsl::comp_hash(semantic);

      switch (semantic_hash)
      {
      case "COLOR"_sid:           return ShaderSemantic::Color;
      case "POSITION"_sid:        return ShaderSemantic::Position;
      case "PSIZE"_sid:           return ShaderSemantic::PSize;
      case "BINORMAL"_sid:        return ShaderSemantic::BiNormal;
      case "BLENDINDICES"_sid:    return ShaderSemantic::BlendIndices;
      case "BLENDWEIGHT"_sid:     return ShaderSemantic::BlendWeight;
      case "NORMAL"_sid:          return ShaderSemantic::Normal;
      case "POSITIONT"_sid:       return ShaderSemantic::PositionT;
      case "TANGENT"_sid:         return ShaderSemantic::Tangent;
      case "TEXCOORD"_sid:        return ShaderSemantic::TexCoord;
      case "FOG"_sid:             return ShaderSemantic::Fog;
      case "TESSFACTOR"_sid:      return ShaderSemantic::TessFactor;
      default: break;
      }

      return invalid_obj<ShaderSemantic>();
    }

    VertexBufferFormat to_vertex_input_format(ShaderArithmeticType type)
    {
      switch (type)
      {
      case rex::gfx::ShaderArithmeticType::Uint:       return VertexBufferFormat::Uint;
      case rex::gfx::ShaderArithmeticType::Float:      return VertexBufferFormat::Float;
      case rex::gfx::ShaderArithmeticType::Float2:     return VertexBufferFormat::Float2;
      case rex::gfx::ShaderArithmeticType::Float3:     return VertexBufferFormat::Float3;
      case rex::gfx::ShaderArithmeticType::Float4:     return VertexBufferFormat::Float4;
      default: break;
      }

      return invalid_obj<VertexBufferFormat>();
    }

    bool operator==(const InputLayoutDesc& lhs, const InputLayoutDesc& rhs)
    {
      if (lhs.size() != rhs.size())
      {
        return false;
      }

      return rsl::memcmp(lhs.data(), rhs.data(), lhs.size() * sizeof(lhs[0])) == 0;
    }
    bool operator!=(const InputLayoutDesc& lhs, const InputLayoutDesc& rhs)
    {
      return !(lhs == rhs);
    }

		InputLayoutDesc create_input_layout_desc_from_reflection(const rsl::vector<ShaderParamReflection>& shaderInputParams)
		{
      rsl::vector<InputLayoutElementDesc> input_element_descriptions(rsl::Size(shaderInputParams.size()));
      REX_ASSERT_X(!input_element_descriptions.empty(), "No input elements provided for input layout");

      s32 byte_offset = 0;
      for (s32 i = 0; i < shaderInputParams.size(); ++i)
      {
        input_element_descriptions[i].semantic = shader_semantic_type(shaderInputParams[i].semantic_name);
        input_element_descriptions[i].format = to_vertex_input_format(shaderInputParams[i].type);
        input_element_descriptions[i].input_slot_class = InputLayoutClassification::PerVertex; // This is hardcoded, I wonder if there's a way around that..
        input_element_descriptions[i].semantic_index = shaderInputParams[i].semantic_index;
        input_element_descriptions[i].input_slot = 0;
        input_element_descriptions[i].aligned_byte_offset = byte_offset;
        input_element_descriptions[i].instance_data_step_rate = 0;

        byte_offset += format_byte_size(input_element_descriptions[i].format);
      }

      return InputLayoutDesc{ input_element_descriptions };
		}

    InputLayout::InputLayout(s32 vertexSize, const InputLayoutDesc& desc)
      : m_vertex_size(vertexSize)
      , m_desc(desc)
    {}

    s32 InputLayout::vertex_size() const
    {
      return m_vertex_size;
    }

    // Validate a given descriptor and see if it can be used with this input layout
    bool InputLayout::validate_desc(const InputLayoutDesc& desc)
    {
      if (desc.size() != m_desc.size())
      {
        return false;
      }

      for (const auto& elem : desc)
      {
        auto it = rsl::find_if(m_desc.cbegin(), m_desc.cend(),
          [&](const InputLayoutElementDesc& myElem)
          {
            return elem.semantic == myElem.semantic;
          });

        if (it == m_desc.cend())
        {
          return false;
        }

        ShaderArithmeticType format = it->format;
        if (!is_convertible_shader_param_type(format, elem.format))
        {
          return false;
        }
      }

      return true;
    }
	}
}