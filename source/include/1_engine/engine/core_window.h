#pragma once

#include <rex_stl/string/string_view.h>

namespace rex
{
    DEFINE_YES_NO_ENUM(FullScreen);

    struct WindowDescription
    {
        WindowDescription()
            : title("rex")
            , width(1280)
            , height(720)
            , fullscreen(FullScreen::No)
        {}

        rtl::StringView title;
        int32 width;
        int32 height;
        FullScreen fullscreen;
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

        virtual int32 width() const = 0;
        virtual int32 height() const = 0;
    };
}