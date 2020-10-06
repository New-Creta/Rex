#pragma once

namespace rex
{
    namespace graphics
    {
        class RenderCommand
        {
        public:
            RenderCommand() = default;
            virtual ~RenderCommand() = default;

            virtual void execute() = 0;

            virtual size_t getCommandTypeID() const = 0;
        };
    }
}