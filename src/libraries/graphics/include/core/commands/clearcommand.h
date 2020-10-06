#pragma once

#include "core/commands/rendercommand.h"

namespace rex
{
    namespace graphics
    {
        class ClearCommand : public RenderCommand
        {
        public:
            ClearCommand();
            ~ClearCommand() override;

            void setClearColor(const glm::vec4& color);
            void setClearDepth(float depth);

            const glm::vec4& getClearColor() const;
            float getClearDepth() const;

            size_t getCommandTypeID() const override;

        private:
            glm::vec4 m_clear_color;
            float m_clear_depth;
        };
    }
}