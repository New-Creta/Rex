#include "rex_engine_pch.h"

#include "core/graphicalapplication.h"
#include "core/layer.h"
#include "core/bufferlayout.h"
#include "core/imguilayer.h"
#include "core/renderer.h"
#include "core/mesh.h"

#include "events/event.h"
#include "events/eventdispatcher.h"
#include "events/window/windowclose.h"

#include "api/opengl/core/glshader.h"
#include "api/opengl/core/glindexbuffer.h"
#include "api/opengl/core/glvertexbuffer.h"
#include "api/opengl/core/glvertexarray.h"

namespace
{
    //-------------------------------------------------------------------------
    rex::graphics::RendererAPI::Type getRenderAPI()
    {
        rex::graphics::RendererAPI::Type type = rex::graphics::RendererAPI::Type::NONE;
#ifdef _OPENGL
        type = rex::graphics::RendererAPI::Type::OPENGL;
#endif
        return type;
    }
}

//-------------------------------------------------------------------------
rex::engine::GraphicalApplication::GraphicalApplication()
    :m_window(nullptr)
    ,m_context(nullptr)
    ,m_layer_stack()
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
rex::graphics::Context* rex::engine::GraphicalApplication::getContext()
{
    return m_context.get();
}
//-------------------------------------------------------------------------
const rex::graphics::Context* rex::engine::GraphicalApplication::getContext() const
{
    return m_context.get();
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

    events::EventDispatcher dispatcher(event);

    dispatcher.dispatch<events::WindowClose>(std::bind(&rex::engine::GraphicalApplication::onWindowClose, this, std::placeholders::_1));
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appInitialize()
{
    m_window = createWindow();
    m_context = createContext();

    pushBackLayer(createImGUILayer());

    Input::createInstance();

    ApplicationContext::makeCurrent(m_context.get());

    graphics::Renderer::initialize(getRenderAPI());

    onInitialize();

    m_window->show();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appUpdate(float dTime)
{
    unsigned int width = m_window->getWidth();
    unsigned int height = m_window->getHeight();

    if (m_window->processEvents())  // Do not do anything when an event is handled.
        return;

    ApplicationContext::makeCurrent(m_context.get());

    Input::instance()->poll();

    m_context->resize(width, height);

    for (const std::unique_ptr<Layer>& layer : m_layer_stack)
    {
        layer->onUpdate();
    }

    onUpdate(dTime);

    for (const std::unique_ptr<Layer>& layer : m_layer_stack)
    {
        ImGUILayer* imgui_layer = dynamic_cast<ImGUILayer*>(layer.get());
        if (imgui_layer == nullptr)
            continue;

        imgui_layer->onBeginRender();
        imgui_layer->onRender();
        imgui_layer->onEndRender();
    }

    m_window->update();
    m_context->swapBuffers();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appQuit()
{
    onQuit();

    Input::destroyInstance();

    m_window->hide();

    ApplicationContext::destroy(m_context.get());
}

//-------------------------------------------------------------------------
bool rex::engine::GraphicalApplication::onWindowClose(events::WindowClose& e)
{
    UNUSED_PARAM(e);

    // This will terminate the application
    markForDestroy();

    return true;
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
    return ApplicationContext::create(m_window->getNativeWindow());
}
//-------------------------------------------------------------------------
std::unique_ptr<rex::engine::ImGUILayer> rex::engine::GraphicalApplication::createImGUILayer()
{
    return std::make_unique<ImGUILayer>();
}
