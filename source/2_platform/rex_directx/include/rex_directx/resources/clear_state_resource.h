#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resources/clear_state.h"

#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
    namespace rhi
    {
        namespace resources
        {
          using ClearState = ClearStateDesc;
        } // namespace resources

        class ClearStateResource : public BaseResource<resources::ClearState>
        {
        public:
            explicit ClearStateResource(ResourceHash hash, const resources::ClearState& cs)
                : BaseResource(&m_clear_state, hash)
                , m_clear_state(cs)
            {}
            ~ClearStateResource() override = default;

        private:
            resources::ClearState m_clear_state;
        };
    } // namespace renderer
} // namespace rex