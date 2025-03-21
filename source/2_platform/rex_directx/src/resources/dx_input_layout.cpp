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

    DxInputLayout::DxInputLayout(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements, const InputLayoutDesc& desc)
      : InputLayout(calc_size(elements), desc)
      , m_input_elements(elements)
    {
      m_input_layout_desc = { m_input_elements.data(), static_cast<u32>(m_input_elements.size()) };
    }

    // Return the wrapped Direct X object
    D3D12_INPUT_LAYOUT_DESC& DxInputLayout::dx_object()
    {
      return m_input_layout_desc;
    }
  }
}