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
    m_layers.push_back(std::move(layer));
}
//-------------------------------------------------------------------------
void rex::engine::LayerStack::pushFront(std::unique_ptr<Layer> layer)
{
    m_layers.insert(std::cbegin(m_layers), std::move(layer));
}
//-------------------------------------------------------------------------
void rex::engine::LayerStack::insert(std::unique_ptr<Layer> layer, int index)
{
    RX_ASSERT_X(index > 0 && index < m_layers.size() - 1, "Index is out of range");

    auto where = std::cbegin(m_layers);

    std::advance(where, index);
    m_layers.insert(where, std::move(layer));
}

//-------------------------------------------------------------------------
void rex::engine::LayerStack::removeLayer(const std::string& layer)
{
    m_layers.erase(std::remove_if(std::begin(m_layers), std::end(m_layers),
        [&layer](const std::unique_ptr<Layer>& l)
        {
            return layer == l->getName();
        }), m_layers.end());
}
//-------------------------------------------------------------------------
void rex::engine::LayerStack::removeLayer(Layer* layer)
{
    m_layers.erase(std::remove_if(std::begin(m_layers), std::end(m_layers),
        [&layer](const std::unique_ptr<Layer>& l)
        {
            return layer == l.get();
        }), m_layers.end());
}

//-------------------------------------------------------------------------
void rex::engine::LayerStack::removeAll()
{
    m_layers.clear();
}