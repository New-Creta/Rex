#pragma once

namespace rex
{
    class IGraphicsContext
    {
    public:
        static IGraphicsContext* get_instance();

        IGraphicsContext();
        virtual ~IGraphicsContext();

        virtual void resize(s32 x, s32 y, s32 width, s32 height) = 0;
        virtual void make_current() = 0;
        virtual void present() = 0;

    private:
        static IGraphicsContext* s_instance;
    };
}