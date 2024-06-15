#include "rex_directx/resources/dx_input_layout.h"

namespace rex
{
  namespace gfx
  {
    DxInputLayout::DxInputLayout(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements)
      : m_input_elements(elements)
    {
      m_input_layout_desc = { m_input_elements.data(), static_cast<u32>(m_input_elements.size()) };
    }

    D3D12_INPUT_LAYOUT_DESC* DxInputLayout::get()
    {
      return &m_input_layout_desc;
    }

  }
}