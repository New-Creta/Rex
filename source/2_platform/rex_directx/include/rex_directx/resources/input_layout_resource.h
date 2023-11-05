#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"

#include "rex_engine/types.h"

#include <rex_std/vector.h>

namespace rex
{
    namespace renderer
    {
        class InputLayoutResource : public BaseResource<rsl::vector<D3D12_INPUT_ELEMENT_DESC>>
        {
        public:
            InputLayoutResource(const rsl::vector<D3D12_INPUT_ELEMENT_DESC>& elements)
                :m_input_elements(elements)
            {}
            ~InputLayoutResource() override = default;

            rsl::vector<D3D12_INPUT_ELEMENT_DESC>* get()
            {
                return &m_input_elements;
            }
            const rsl::vector<D3D12_INPUT_ELEMENT_DESC>* get() const
            {
                return &m_input_elements;
            }

        private:
            rsl::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements;
        };
    }
}