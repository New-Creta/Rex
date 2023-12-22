#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct AttachCommitedResourceToFrameCommandDesc
            {
                const ResourceSlot* frame_slot = nullptr;

                s32 buffer_count;
                s32 buffer_byte_size;
            };

            class AttachCommitedResourceToFrame : public RenderCommand
            {
            public:
                AttachCommitedResourceToFrame(AttachCommitedResourceToFrameCommandDesc&& desc, const ResourceSlot& slot)
                    : RenderCommand()
                    , m_desc(rsl::move(desc))
                    , m_resource_slot(slot)
                {}

                ~AttachCommitedResourceToFrame() override = default;

                bool execute() override
                {
                    return backend::attach_commited_resource_to_frame(m_desc, m_resource_slot);
                }

            private:
                AttachCommitedResourceToFrameCommandDesc m_desc;
                ResourceSlot m_resource_slot;
            };
        }
    }
}