#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resources/clear_state.h"

#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
    namespace rhi
    {
        class ClearStateResource : public BaseResource<ClearStateDesc>
        {
        public:
            explicit ClearStateResource(ResourceHash hash, const ClearStateDesc& cs)
                : BaseResource(&m_clear_state, hash)
                , m_clear_state(cs)
            {}
            ~ClearStateResource() override = default;

        private:
            ClearStateDesc m_clear_state;
        };
    } // namespace renderer
} // namespace rex