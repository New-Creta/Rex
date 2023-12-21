#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetConstantBufferViewCommandDesc
            {
                SetConstantBufferViewCommandDesc()
                    :location(0)
                {}

                SetConstantBufferViewCommandDesc(s32 constantBufferLocation)
                    :location(constantBufferLocation)
                {}

                s32 location;
            };
            class SetConstantBufferView : public RenderCommand
            {
            public:
                SetConstantBufferView(SetConstantBufferViewCommandDesc&& desc, ResourceSlot slot)
                    : RenderCommand()
                    , m_desc(rsl::move(desc))
                    , m_resource_slot(slot)
                {}

                ~SetConstantBufferView() override = default;

                bool execute() override
                {
                    return backend::set_constant_buffer_view(m_resource_slot, m_desc.location);
                }

            private:
                SetConstantBufferViewCommandDesc m_desc;
                ResourceSlot m_resource_slot;
            };
        } // namespace commands
    }   // namespace renderer
} // namespace rex