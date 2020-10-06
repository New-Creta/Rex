#pragma once

#include "core/commands/rendercommand.h"

namespace rex
{
    namespace graphics
    {
        class DrawCommand : public RenderCommand
        {
        public:
            DrawCommand();
            ~DrawCommand() override;

            size_t getCommandTypeID() const override;
        };
    }
}