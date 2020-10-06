#pragma once

#include "core/commands/clearcommand.h"

namespace rex
{
    namespace opengl
    {
        class ClearCommand : public graphics::ClearCommand
        {
        public:
            ClearCommand();
            ClearCommand(const glm::vec4& clearColor, float clearDepth);
            ~ClearCommand() override;

            void execute() override;
        };
    }
}