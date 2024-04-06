#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace rhi
  {
    class InputLayoutResource : public BaseResource<D3D12_INPUT_LAYOUT_DESC>
    {
    public:
      explicit InputLayoutResource(ResourceHash hash, const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements)
          : BaseResource(&m_input_layout_desc, hash)
          , m_input_elements(elements)
      {
        m_input_layout_desc = {m_input_elements.data(), static_cast<u32>(m_input_elements.size())};
      }
      ~InputLayoutResource() override = default;

    private:
      rsl::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements {};
      D3D12_INPUT_LAYOUT_DESC m_input_layout_desc;
    };
  } // namespace rhi
} // namespace rex