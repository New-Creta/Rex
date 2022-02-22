#pragma once

namespace rex
{
    DEFINE_YES_NO_ENUM(FullScreen);

    struct WindowDescription
    {
        WindowDescription()
            :title("rex")
            , width(1280)
            , height(720)
            , fullscreen(FullScreen::No)
        {}

        std::string title;
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

        virtual void make_windowed(int32 newWidth = -1, int32 newHeight = -1) = 0;
        virtual void make_fullscreen(int32 newWidth = -1, int32 newHeight = -1) = 0;

        virtual int32 get_width() const = 0;
        virtual int32 get_height() const = 0;
    };
}