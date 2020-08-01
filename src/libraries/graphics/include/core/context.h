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

        protected:
            virtual bool create() = 0;
            virtual bool destroy() = 0;
             
            virtual void setAsCurrent() = 0;
        };
    }
}