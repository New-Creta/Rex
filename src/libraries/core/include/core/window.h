#pragma once

#include "rex_core_global.h"

namespace rex
{
    namespace events
    {
        class Event;
    }

    struct WindowProperties
    {
        using EventCallbackFn = std::function<void(const events::Event&)>;

        //-------------------------------------------------------------------------
        WindowProperties()
            :title("Rex")
            ,width(1280)
            ,height(720)
        {}
        //-------------------------------------------------------------------------
        WindowProperties(const std::string& wndTitle, unsigned int wndWidth, unsigned int wndHeight)
            :title(wndTitle)
            ,width(wndWidth)
            ,height(wndHeight)
        {}
        //-------------------------------------------------------------------------
        WindowProperties(const std::string& wndTitle, unsigned int wndWidth, unsigned int wndHeight, EventCallbackFn callback)
            :title(wndTitle)
            ,width(wndWidth)
            ,height(wndHeight)
            ,event_callback(callback)
        {}

        const std::string title;
        
        unsigned int width;
        unsigned int height;

        EventCallbackFn event_callback;
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(const events::Event&)>;

        REX_CORE_EXPORT Window() = default;
        REX_CORE_EXPORT virtual ~Window() = default;

        virtual void show() = 0;
        virtual void hide() = 0;

        virtual void update() = 0;

        virtual unsigned int getWidth() const = 0;
        virtual unsigned int getHeight() const = 0;

        virtual void* getHandle() = 0;
    };
}