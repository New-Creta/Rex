#pragma once

#include "rex_std/string_view.h"
#include "rex_std_extra/utilities/yes_no_enum.h"

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

        rsl::string_view title;
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