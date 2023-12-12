#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct NewFrameCommandDesc
            {
                RenderCommandDesc command;
            };

            class NewFrame : public RenderCommand
            {
            public:
                NewFrame(NewFrameCommandDesc&& desc)
                    :RenderCommand(rsl::move(desc.command))
                    ,m_desc(rsl::move(desc))
                {}

                ~NewFrame() override = default;

                bool execute() override
                {
                    return backend::new_frame();
                }

            private:
                NewFrameCommandDesc m_desc;
            }
        }
    }
}