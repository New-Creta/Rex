#include "core/entrypoint.h"

#include "core/application.h"
#include "core/win32window.h"

#include "events/event.h"

//-------------------------------------------------------------------------
sandbox::Application::Application() = default;
//-------------------------------------------------------------------------
sandbox::Application::~Application() = default;

//-------------------------------------------------------------------------
std::unique_ptr<rex::Window> sandbox::Application::createWindow()
{
    const std::string WINDOW_TITLE = "Rex";

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    rex::WindowProperties properties =
    {
        WINDOW_TITLE,

        WINDOW_WIDTH,
        WINDOW_HEIGHT,

        [this](const rex::events::Event& event) { rex::CoreApplication::onEvent(event); }
    };

    return std::make_unique<rex::win32::Window>(properties);
}

//-------------------------------------------------------------------------
rex::CoreApplication* rex::createApplication()
{
    return new sandbox::Application();
}