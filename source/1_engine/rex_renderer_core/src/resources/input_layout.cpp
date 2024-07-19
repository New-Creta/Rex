#include "rex_renderer_core/resources/input_layout.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/engine/invalid_object.h"

namespace rex
{
	namespace gfx
	{
    VertexBufferFormat to_vertex_input_format(ShaderParameterType type)
    {
      switch (type)
      {
      case rex::gfx::ShaderParameterType::Uint:       return VertexBufferFormat::Uint;
      case rex::gfx::ShaderParameterType::Float:      return VertexBufferFormat::Float;
      case rex::gfx::ShaderParameterType::Float2:     return VertexBufferFormat::Float2;
      case rex::gfx::ShaderParameterType::Float3:     return VertexBufferFormat::Float3;
      case rex::gfx::ShaderParameterType::Float4:     return VertexBufferFormat::Float4;
      }

      invalid_obj<VertexBufferFormat>();
    }

		InputLayoutDesc create_input_layout_desc_from_reflection(const rsl::vector<ShaderParamReflection>& shaderInputParams)
		{
      rsl::vector<InputLayoutElementDesc> input_element_descriptions(rsl::Size(shaderInputParams.size()));
      REX_ASSERT_X(!input_element_descriptions.empty(), "No input elements provided for input layout");

      s32 byte_offset = 0;
      for (s32 i = 0; i < shaderInputParams.size(); ++i)
      {
        input_element_descriptions[i].semantic_name = shaderInputParams[i].semantic_name;
        input_element_descriptions[i].format = to_vertex_input_format(shaderInputParams[i].type);
        input_element_descriptions[i].input_slot_class = InputLayoutClassification::PerVertexData; // This is hardcoded, I wonder if there's a way around that..
        input_element_descriptions[i].semantic_index = shaderInputParams[i].semantic_index;
        input_element_descriptions[i].input_slot = 0;
        input_element_descriptions[i].aligned_byte_offset = byte_offset;
        input_element_descriptions[i].instance_data_step_rate = 0;

        byte_offset += format_byte_size(input_element_descriptions[i].format);
      }

      return InputLayoutDesc{ input_element_descriptions };
		}

    InputLayout::InputLayout(s32 vertexSize, InputLayoutDesc&& desc)
      : m_vertex_size(vertexSize)
      , m_desc(rsl::move(desc))
    {}

    s32 InputLayout::vertex_size() const
    {
      return m_vertex_size;
    }

    // Validate a given descriptor and see if it can be used with this input layout
    bool InputLayout::validate_desc(const InputLayoutDesc& desc)
    {
      if (desc.input_layout.size() != m_desc.input_layout.size())
      {
        return false;
      }

      for (const auto& elem : desc.input_layout)
      {
        auto it = rsl::find_if(m_desc.input_layout.cbegin(), m_desc.input_layout.cend(),
          [&](const InputLayoutElementDesc& myElem)
          {
            return elem.semantic_name == myElem.semantic_name;
          });

        if (it == m_desc.input_layout.cend())
        {
          return false;
        }

        ShaderParameterType format = it->format;
        if (!is_convertible_shader_param_type(format, elem.format))
        {
          return false;
        }
      }

      return true;
    }
	}
}