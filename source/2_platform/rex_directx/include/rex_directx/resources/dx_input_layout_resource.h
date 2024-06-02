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
        class DxInputLayoutResource : public InputLayout
        {
        public:
            explicit DxInputLayoutResource(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements)
                : m_input_elements(elements)
            {
              m_input_layout_desc = { m_input_elements.data(), static_cast<u32>(m_input_elements.size()) };
            }

            D3D12_INPUT_LAYOUT_DESC* get()
            {
              return &m_input_layout_desc;
            }

        private:
            rsl::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements{};
            D3D12_INPUT_LAYOUT_DESC m_input_layout_desc;
        };
    } // namespace renderer
} // namespace rex