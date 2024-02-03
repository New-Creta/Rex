#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

#include "rex_std/bonus/memory/stack_allocator.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct NewFrameCommandDesc
            {
            };

            class NewFrame : public RenderCommand
            {
            public:
              explicit NewFrame(NewFrameCommandDesc&& desc)
                  : RenderCommand()
                  , m_desc(rsl::move(desc))
              {
              }

              ~NewFrame() override = default;

              bool execute() override
              {
                return backend::new_frame();
              }

            private:
              NewFrameCommandDesc m_desc;
            };
        }
    }
}