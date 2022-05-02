#pragma once

namespace rex
{
    class CoreApplicationLoop
    {
    public:
        virtual ~CoreApplicationLoop() = default;

        virtual void exec() = 0;
        virtual void stop() = 0;

    protected:
        virtual void tick() = 0;
    };
}