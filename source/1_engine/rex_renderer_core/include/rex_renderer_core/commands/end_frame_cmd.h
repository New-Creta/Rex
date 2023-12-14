#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct EndFrameCommandDesc
            {
            };

            class EndFrame : public RenderCommand
            {
            public:
              EndFrame(EndFrameCommandDesc&& desc)
                  : RenderCommand()
                  , m_desc(rsl::move(desc))
              {
              }

              ~EndFrame() override = default;

              bool execute() override
              {
                return backend::end_frame();
              }

            private:
              EndFrameCommandDesc m_desc;
            };
        }
    }
}