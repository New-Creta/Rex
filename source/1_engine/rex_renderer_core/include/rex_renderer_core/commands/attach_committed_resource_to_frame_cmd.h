#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct AttachCommittedResourceToFrameCommandDesc
            {
                const ResourceSlot* frame_slot = nullptr;

                s32 buffer_count;
                s32 buffer_byte_size;
            };

            class AttachCommittedResourceToFrame : public RenderCommand
            {
            public:
                AttachCommittedResourceToFrame(AttachCommittedResourceToFrameCommandDesc&& desc, const ResourceSlot& slot)
                    : RenderCommand()
                    , m_desc(rsl::move(desc))
                    , m_resource_slot(slot)
                {}

                ~AttachCommittedResourceToFrame() override = default;

                bool execute() override
                {
                    return backend::attach_committed_resource_to_frame(m_desc, m_resource_slot);
                }

            private:
                AttachCommittedResourceToFrameCommandDesc m_desc;
                ResourceSlot m_resource_slot;
            };
        }
    }
}