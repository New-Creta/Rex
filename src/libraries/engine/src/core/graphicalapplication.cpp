#include "rex_engine_pch.h"

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
rex::Window* rex::engine::GraphicalApplication::getWindow()
{
    return m_window.get();
}
//-------------------------------------------------------------------------
const rex::Window* rex::engine::GraphicalApplication::getWindow() const
{
    return m_window.get();
}

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
void rex::engine::GraphicalApplication::onEvent(events::Event & event)
{
    RX_TRACE(event.toString());

    for (auto it = m_layer_stack.crbegin(); it != m_layer_stack.rend() && event.isHandled() == false; ++it)
    {
        (*it)->onEvent(event);
    }
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appInitialize()
{
    m_window = createWindow();
    m_context = createContext();

    Input::createInstance();

#ifdef _OPENGL
    ApplicationContext::makeCurrent(m_context.get());

    if (glewInit() != GLEW_OK)
    {
        RX_ERROR("Failed to init GLEW");
    }
#endif

    m_window->show();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appUpdate(float dTime)
{
    UNUSED_PARAM(dTime);

    unsigned int width = m_window->getWidth();
    unsigned int height = m_window->getHeight();

    m_window->processEvents();

    ApplicationContext::makeCurrent(m_context.get());

#ifdef _OPENGL
    glViewport(0, 0, width, height);
    glClearColor(1, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    for (const std::unique_ptr<Layer>& layer : m_layer_stack)
    {
        layer->onUpdate();
    }

    m_window->update();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appQuit()
{
    Input::destroyInstance();

    m_window->hide();

    ApplicationContext::destroy(m_context.get());
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
std::unique_ptr<rex::engine::ApplicationContext> rex::engine::GraphicalApplication::createContext()
{
    return ApplicationContext::create(m_window->getHandle());
}
