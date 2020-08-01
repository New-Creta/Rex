#include "rex_engine_pch.h"

#include "core/layerstack.h"
#include "core/layer.h"

//-------------------------------------------------------------------------
rex::engine::LayerStack::LayerStack() = default;
//-------------------------------------------------------------------------
rex::engine::LayerStack::~LayerStack() = default;

//-------------------------------------------------------------------------
void rex::engine::LayerStack::pushBack(std::unique_ptr<Layer> layer)
{
    layer->onAttach();
    m_layers.push_back(std::move(layer));
}
//-------------------------------------------------------------------------
void rex::engine::LayerStack::pushFront(std::unique_ptr<Layer> layer)
{
    layer->onAttach();
    m_layers.insert(std::cbegin(m_layers), std::move(layer));
}
//-------------------------------------------------------------------------
void rex::engine::LayerStack::insert(std::unique_ptr<Layer> layer, int index)
{
    RX_ASSERT_X(index > 0 && index < m_layers.size() - 1, "Index is out of range");

    auto where = std::cbegin(m_layers);

    std::advance(where, index);
    m_layers.insert(where, std::move(layer));
    layer->onAttach();
}

//-------------------------------------------------------------------------
void rex::engine::LayerStack::removeLayer(const std::string & layer)
{
    auto it = std::find_if(std::cbegin(m_layers), std::cend(m_layers),
        [&layername = layer](const std::unique_ptr<Layer>& layer)
        {
            return layername == layer->getName();
        });

    if (it == std::cend(m_layers))
        return;

    (*it)->onDetach();
    m_layers.erase(it);
}
//-------------------------------------------------------------------------
void rex::engine::LayerStack::removeLayer(Layer * layer)
{
    auto it = std::find_if(std::cbegin(m_layers), std::cend(m_layers),
        [layer](const std::unique_ptr<Layer>& l)
        {
            return layer == l.get();
        });

    if (it == std::cend(m_layers))
        return;

    (*it)->onDetach();
    m_layers.erase(it);
}

//-------------------------------------------------------------------------
void rex::engine::LayerStack::removeAll()
{
    m_layers.clear();
}