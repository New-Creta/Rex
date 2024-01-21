#pragma once

#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct CreateConstantBufferViewCommandDesc
            {
                CreateConstantBufferViewCommandDesc()
                    :frame_index(0)
                    ,committed_resource()
                    ,buffer_size(0)
                {}

                s32 frame_index;
                ResourceSlot committed_resource;

                s32 buffer_size;
            };

            class CreateConstantBufferView : public RenderCommand
            {
            public:
                CreateConstantBufferView(CreateConstantBufferViewCommandDesc&& desc, const ResourceSlot& slot)
                    : RenderCommand()
                    , m_desc(rsl::move(desc))
                    , m_resource_slot(slot)
                {}

                ~CreateConstantBufferView() override = default;

                bool execute() override
                {
                    return backend::create_constant_buffer_view(m_desc, m_resource_slot);
                }

            private:
                CreateConstantBufferViewCommandDesc m_desc;
                ResourceSlot m_resource_slot;
            };
        } // namespace commands
    }   // namespace renderer
} // namespace rex