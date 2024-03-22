#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_renderer_core/resource_management/resource.h"

#include "rex_engine/engine/types.h"

#include "rex_std/vector.h"

namespace rex
{
    namespace rhi
    {
        class InputLayoutResource : public BaseResource<rsl::vector<D3D12_INPUT_ELEMENT_DESC>>
        {
        public:
            explicit InputLayoutResource(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements)
                : BaseResource(&m_input_elements)
                ,m_input_elements(elements)
            {}
            ~InputLayoutResource() override = default;

        private:
            rsl::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements{};
        };
    } // namespace renderer
} // namespace rex