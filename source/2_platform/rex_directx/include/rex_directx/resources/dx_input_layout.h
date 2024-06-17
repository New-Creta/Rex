#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resources/input_layout.h"

#include "rex_engine/engine/types.h"

#include "rex_std/vector.h"

namespace rex
{
  namespace gfx
  {
    class DxInputLayout : public InputLayout
    {
    public:
      explicit DxInputLayout(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements);

      D3D12_INPUT_LAYOUT_DESC* dx_object();

    private:
      rsl::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements{};
      D3D12_INPUT_LAYOUT_DESC m_input_layout_desc;
    };
  } // namespace gfx
} // namespace rex