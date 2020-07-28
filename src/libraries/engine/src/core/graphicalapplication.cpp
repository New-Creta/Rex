#include "core/graphicalapplication.h"

#include "events/event.h"
#include "events/input/keydown.h"

//-------------------------------------------------------------------------
rex::engine::GraphicalApplication::GraphicalApplication()
    :m_window(nullptr)
{}
//-------------------------------------------------------------------------
rex::engine::GraphicalApplication::~GraphicalApplication() = default;

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::onEvent(const events::Event& event)
{
    RX_INFO(event.toString());

    if (event.getType() == events::EventType::KEY_PRESSED)
    {
        const events::KeyDown& keydown = static_cast<const events::KeyDown&>(event);
        if (keydown.getKeyCode() == KeyCode::ESCAPE)
        {
            quit();
        }
    }
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appInitialize()
{
    m_window = createWindow();
    m_window->show();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appUpdate(float dTime)
{
    UNUSED_PARAM(dTime);

    m_window->update();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appQuit()
{
    m_window->hide();
}

//-------------------------------------------------------------------------
rex::engine::ApplicationWindow* rex::engine::GraphicalApplication::getWindow()
{
    return m_window.get();
}
//-------------------------------------------------------------------------
const rex::engine::ApplicationWindow* rex::engine::GraphicalApplication::getWindow() const
{
    return m_window.get();
}


//-------------------------------------------------------------------------
std::unique_ptr<rex::engine::ApplicationWindow> rex::engine::GraphicalApplication::createWindow()
{
    const std::string WINDOW_TITLE = "Rex";

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    rex::WindowProperties properties = 
    {
        WINDOW_TITLE,
    
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
    
        [this](const events::Event& event) { GraphicalApplication::onEvent(event); }
    };

    return std::make_unique<ApplicationWindow>(properties);
}
