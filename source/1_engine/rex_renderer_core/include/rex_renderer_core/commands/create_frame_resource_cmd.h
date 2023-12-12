#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct CreateFrameResourceCommandDesc
            {
                
            };

            class CreateFrameResource : public RenderCommand
            {
            public:
                CreateFrameResource(CreateFrameResourceCommandDesc&& desc, ResourceSlot slot)
                    :RenderCommand()
                    ,m_desc(rsl::move(desc))
                    ,m_resource_slot(slot)
                {}

                ~CreateFrameResource() override = default;

                bool execute() override
                {
                    return backend::create_frame_resource(m_resource_slot);
                }

            private:
                CreateFrameResourceCommandDesc m_desc;
                ResourceSlot m_resource_slot;
            }
        }
    }
}