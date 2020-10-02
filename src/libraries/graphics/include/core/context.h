#pragma once

namespace rex
{
    class Context
    {
    public:
        Context() = default;
        virtual ~Context() = default;

        virtual void* getNativeContext() const = 0;

    protected:
        virtual bool create() = 0;
        virtual bool destroy() = 0;

        virtual void setAsCurrent() = 0;
    };
}