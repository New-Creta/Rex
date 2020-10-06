#include "rex_engine_pch.h"

#include "core/layer.h"

//-------------------------------------------------------------------------
rex::engine::Layer::Layer(const std::string& name /*= "Layer"*/)
    :m_name(name)
    , m_enabled(true)
{}
//-------------------------------------------------------------------------
rex::engine::Layer::~Layer() = default;

//-------------------------------------------------------------------------
void rex::engine::Layer::enable()
{
    m_enabled = true;
}
//-------------------------------------------------------------------------
void rex::engine::Layer::disable()
{
    m_enabled = false;
}

//-------------------------------------------------------------------------
void rex::engine::Layer::onUpdate()
{
    // Nothing to implement
}
//-------------------------------------------------------------------------
void rex::engine::Layer::onEvent(events::Event & event)
{
    UNUSED_PARAM(event);

    // Nothing to implement
}

