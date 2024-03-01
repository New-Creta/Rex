#pragma once

#include "rex_directx/directx_util.h"
#include "rex_renderer_resources/resource.h"

#include "rex_engine/engine/types.h"

#include "rex_std/vector.h"

namespace rex
{
    namespace renderer
    {
        class InputLayoutResource : public BaseResource<rsl::vector<D3D12_INPUT_ELEMENT_DESC>>
        {
        public:
            RESOURCE_CLASS_TYPE(InputLayoutResource);

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