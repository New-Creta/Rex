#include "rex_engine_pch.h"

#include "layer.h"

#include "frameinfo/frameinfo.h"

namespace rex
{
    //-------------------------------------------------------------------------
    Layer::Layer(const std::string& name, StackOrder order)
        : m_name(name.empty() ? "Layer" : name)
        , m_order(order)
        , m_enabled(true)
    {
    }
    //-------------------------------------------------------------------------
    Layer::~Layer() = default;

    //-------------------------------------------------------------------------
    void Layer::enable()
    {
        m_enabled = true;
    }
    //-------------------------------------------------------------------------
    void Layer::disable()
    {
        m_enabled = false;
    }

    //-------------------------------------------------------------------------
    void Layer::attach()
    {
        on_attach();
    }

    //-------------------------------------------------------------------------
    void Layer::detach()
    {
        on_detach();
    }

    //-------------------------------------------------------------------------
    void Layer::update(const FrameInfo& info)
    {
        on_update(info);
    }

    //-------------------------------------------------------------------------
    void Layer::handle_event()
    {
        on_event();
    }

    //-------------------------------------------------------------------------
    const std::string& Layer::get_name() const
    {
        return m_name;
    }
    //-------------------------------------------------------------------------
    const Layer::StackOrder Layer::get_order() const
    {
        return m_order;
    }
    //-------------------------------------------------------------------------
    bool Layer::is_enabled() const
    {
        return m_enabled;
    }

    //-------------------------------------------------------------------------
    void Layer::on_update(const FrameInfo& /*info*/)
    {
        // UNUSED_PARAM(info);
    }

    //-------------------------------------------------------------------------
    void Layer::on_event()
    {
        // Nothing to implement
    }
}