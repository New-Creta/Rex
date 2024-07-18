#include "rex_directx/resources/dx_input_layout.h"

#include "rex_std/numeric.h"

namespace rex
{
  namespace gfx
  {
    // We expect that all elements are packed together without alignment between them
    s32 calc_size(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements)
    {
      s32 total_size = 0;
      for (const auto& elem : elements)
      {
        total_size += d3d::format_byte_size(elem.Format);
      }

      return total_size;
    }

    DxInputLayout::DxInputLayout(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements)
      : InputLayout(calc_size(elements))
      , m_input_elements(elements)
    {
      m_input_layout_desc = { m_input_elements.data(), static_cast<u32>(m_input_elements.size()) };
    }

    // Return the wraped Direct X object
    D3D12_INPUT_LAYOUT_DESC* DxInputLayout::dx_object()
    {
      return &m_input_layout_desc;
    }

    // Validate a given descriptor and see if it can be used with this input layout
    bool DxInputLayout::validate_desc(const InputLayoutDesc& desc)
    {
      if (desc.input_layout.size() != m_input_elements.size())
      {
        return false;
      }

      for (const auto& elem : desc.input_layout)
      {
        auto it = rsl::find_if(m_input_elements.cbegin(), m_input_elements.cend(),
          [&](const D3D12_INPUT_ELEMENT_DESC& d3dElem)
          {
            rsl::string_view semantic_name(d3dElem.SemanticName);
            return semantic_name == elem.semantic_name && static_cast<s32>(d3dElem.SemanticIndex) == elem.semantic_index;
          });

        if (it == m_input_elements.cend())
        {
          return false;
        }

        ShaderParameterType format = d3d::from_dx12_shader_param_type(it->Format);
        if (!is_convertible_shader_param_type(format, elem.format))
        {
          return false;
        }
      }

      return true;
    }

  }
}