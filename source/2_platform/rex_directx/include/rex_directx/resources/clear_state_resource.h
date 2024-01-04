#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_renderer_core/clear_bits.h"
#include "rex_std_extra/math/color.h"
#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct ClearState
            {
                rsl::Color4f rgba;
                f32 depth;
                u8 stencil;
                ClearBits flags;
            };
        }

        class ClearStateResource : public BaseResource<resources::ClearState>
        {
        public:
            RESOURCE_CLASS_TYPE(ClearStateResource);

            ClearStateResource(const resources::ClearState& cs)
                :BaseResource(&m_clear_state)
                ,m_clear_state(cs)
            {}
            ~ClearStateResource() override = default;

        private:
            resources::ClearState m_clear_state;
        };
    }
}