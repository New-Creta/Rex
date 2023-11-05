#pragma once

#include "rex_directx/resources/resource.h"
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
            ClearStateResource(const resources::ClearState& cs)
                :m_clear_state(cs)
            {}
            ~ClearStateResource() override = default;

            resources::ClearState* get()
            {
                return &m_clear_state;
            }
            const resources::ClearState* get() const
            {
                return &m_clear_state;
            }

        private:
            resources::ClearState m_clear_state;
        };
    }
}