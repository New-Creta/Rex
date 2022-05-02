#pragma once

#include "event.h"

namespace rex
{
    class Display;

    struct WindowDescription
    {
        WindowDescription()
            : title("rex")
            , width(1280)
            , height(720)
            , display(nullptr)
        {}

        rtl::StringView title;
        
        int32 width;
        int32 height;

        const Display* display;
        events::EventCallbackFn event_callback;
    };

    class CoreWindow
    {
    public:
        virtual ~CoreWindow();

        virtual void update() = 0;

        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void focus() = 0;
        virtual void close() = 0;

        virtual bool is_visible() const = 0;

        virtual int32 width() const = 0;
        virtual int32 height() const = 0;

        virtual int32 get_id() const = 0;
    };
}