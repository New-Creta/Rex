#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct FinishUserInitializationCommandDesc
            {
            
            };

            class FinishUserInitialization : public RenderCommand
            {
            public:
                FinishUserInitialization(FinishUserInitializationCommandDesc&& desc)
                    :RenderCommand()
                    , m_desc(rsl::move(desc))
                {}

                ~FinishUserInitialization() override = default;

                bool execute() override
                {
                    return backend::finish_user_initialization();
                }

            private:
                FinishUserInitializationCommandDesc m_desc;
            }
        }
    }
}