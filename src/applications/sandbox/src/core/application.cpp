#include "core/entrypoint.h"
#include "core/application.h"
#include "core/layer.h"

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
sandbox::Application::Application()
{
    pushBackLayer(std::make_unique<ExampleLayer>());
}
//-------------------------------------------------------------------------
sandbox::Application::~Application() = default;

//-------------------------------------------------------------------------
rex::CoreApplication* rex::createApplication()
{
    return new sandbox::Application();
}