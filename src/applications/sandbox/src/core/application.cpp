#include "core/entrypoint.h"
#include "core/application.h"
#include "core/layer.h"
#include "core/imguilayer.h"

#include "events/event.h"

class ExampleLayer : public rex::engine::Layer 
{
public:
    ExampleLayer()
        :Layer("Example")
    {}

    //-------------------------------------------------------------------------
    void onUpdate() override
    {
        // Nothing to implement
    }
    //-------------------------------------------------------------------------
    void onEvent(rex::events::Event& event)
    {
        // Nothing to implement
    }
};

//-------------------------------------------------------------------------
sandbox::Application::Application() = default;
//-------------------------------------------------------------------------
sandbox::Application::~Application() = default;

//-------------------------------------------------------------------------
void sandbox::Application::appInitialize()
{
    rex::engine::GraphicalApplication::appInitialize();

    pushBackLayer(std::make_unique<ExampleLayer>());
    pushBackLayer(std::make_unique<rex::engine::ImGUILayer>());
}

//-------------------------------------------------------------------------
rex::CoreApplication* rex::createApplication()
{
    return new sandbox::Application();
}