#pragma once

namespace rex
{
    namespace graphics
    {
        class Context
        {
        public:
            Context() = default;
            virtual ~Context() = default;

            virtual void resize(int width, int height) = 0;
            virtual void swapBuffers() = 0;

        protected:
            virtual bool create() = 0;
            virtual bool destroy() = 0;
             
            virtual void setAsCurrent() = 0;
        };
    }
}