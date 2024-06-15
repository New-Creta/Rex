#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_renderer_core/resources/input_layout.h"

#include "rex_engine/engine/types.h"

#include "rex_std/vector.h"

namespace rex
{
  namespace rhi
  {
    class DxInputLayout : public InputLayout
    {
    public:
      explicit DxInputLayout(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements);

      D3D12_INPUT_LAYOUT_DESC* get();

    private:
      rsl::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements{};
      D3D12_INPUT_LAYOUT_DESC m_input_layout_desc;
    };
  } // namespace renderer
} // namespace rex