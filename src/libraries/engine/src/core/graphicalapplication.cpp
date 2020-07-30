#include "core/graphicalapplication.h"
#include "core/layer.h"

#include "events/event.h"
#include "events/input/keydown.h"

#include "events/eventdispatcher.h"

//-------------------------------------------------------------------------
rex::engine::GraphicalApplication::GraphicalApplication()
    :m_window(nullptr)
{}
//-------------------------------------------------------------------------
rex::engine::GraphicalApplication::~GraphicalApplication() = default;

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::pushBackLayer(std::unique_ptr<Layer> layer)
{
    m_layer_stack.pushBack(std::move(layer));
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::pushFrontLayer(std::unique_ptr<Layer> layer)
{
    m_layer_stack.pushFront(std::move(layer));
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::onEvent(events::Event& event)
{
    RX_TRACE(event.toString());

    for (auto it = m_layer_stack.crbegin(); it != m_layer_stack.rend() && event.isHandled() == false; ++it)
    {
        (*it)->onEvent(event);
    }

    events::EventDispatcher dispatcher(event);

    dispatcher.dispatch<events::KeyDown>(std::bind(&rex::engine::GraphicalApplication::escapeButtonPressed, this, std::placeholders::_1));
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appInitialize()
{
    m_window = createWindow();
    m_context = createContext(m_window->getHandle());

    m_window->show();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appUpdate(float dTime)
{
    UNUSED_PARAM(dTime);

    m_window->update();

    for (const std::unique_ptr<Layer>& layer : m_layer_stack)
    {
        layer->onUpdate();
    }
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
bool rex::engine::GraphicalApplication::escapeButtonPressed(events::KeyDown& keyEvent)
{
    if (keyEvent.getKeyCode() == KeyCode::ESCAPE)
    {
        quit();
        return true;
    }

    return false;
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
    
        [this](events::Event& event) { GraphicalApplication::onEvent(event); }
    };

    return std::make_unique<ApplicationWindow>(properties);
}
//-------------------------------------------------------------------------
std::unique_ptr<rex::engine::ApplicationContext> rex::engine::GraphicalApplication::createContext(void* handle)
{
    auto context = std::make_unique<ApplicationContext>(gl::DriverData(handle));
    context->initialize();
    return context;
}
