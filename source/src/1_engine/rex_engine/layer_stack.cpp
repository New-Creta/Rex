#include "rex_engine_pch.h"

#include "layer_stack.h"
#include "layer.h"

namespace rex
{
    const size_t INITIAL_LAYERSTACK_CAPACITY = 5;

    //-------------------------------------------------------------------------
    LayerStack::LayerStack(int32 initialSize)
    {
        int32 size = initialSize == -1 ? INITIAL_LAYERSTACK_CAPACITY : initialSize;

        //R_ASSERT_X(size >= 0, "Layerstack size cannot be negative!");

        m_layers.reserve(size);
    }
    //-------------------------------------------------------------------------
    LayerStack::~LayerStack()
    {
        //R_ASSERT_X(m_layers.size() == 0, "Layer stack was not cleared!");

        clear();
    }

    //-------------------------------------------------------------------------
    LayerStack::LayerStackIt LayerStack::begin()
    {
        return m_layers.begin();
    }
    //-------------------------------------------------------------------------
    LayerStack::ConstLayerStackIt LayerStack::begin() const
    {
        return m_layers.begin();
    }
    //-------------------------------------------------------------------------
    LayerStack::ConstLayerStackIt LayerStack::cbegin() const
    {
        return begin();
    }
    //-------------------------------------------------------------------------
    LayerStack::LayerStackIt LayerStack::end()
    {
        return m_layers.end();
    }
    //-------------------------------------------------------------------------
    LayerStack::ConstLayerStackIt LayerStack::end() const
    {
        return m_layers.end();
    }
    //-------------------------------------------------------------------------
    LayerStack::ConstLayerStackIt LayerStack::cend() const
    {
        return end();
    }

    //-------------------------------------------------------------------------
    LayerStack::ReverseLayerStackIt LayerStack::rbegin()
    {
        return m_layers.rbegin();
    }
    //-------------------------------------------------------------------------
    LayerStack::ConstReverseLayerStackIt LayerStack::crbegin() const
    {
        return m_layers.crbegin();
    }
    //-------------------------------------------------------------------------
    LayerStack::ReverseLayerStackIt LayerStack::rend()
    {
        return m_layers.rend();
    }
    //-------------------------------------------------------------------------
    LayerStack::ConstReverseLayerStackIt LayerStack::crend() const
    {
        return m_layers.crend();
    }

    //-------------------------------------------------------------------------
    void LayerStack::push(rtl::UniquePtr<Layer> layer)
    {
        layer->attach();
        m_layers.push_back(std::move(layer));

        std::sort(std::begin(m_layers), std::end(m_layers), [](const rtl::UniquePtr<Layer>& layer1, const rtl::UniquePtr<Layer>& layer2)
                  {
                      return layer1->get_order() < layer2->get_order();
                  });
    }

    //-------------------------------------------------------------------------
    void LayerStack::remove_layer(const std::string& layer)
    {
        auto it = std::find_if(std::begin(m_layers), std::end(m_layers), [& layername = layer](const rtl::UniquePtr<Layer>& layer)
                               {
                                   return layername == layer->get_name();
                               });

        if (it == std::end(m_layers))
            return;

        (*it)->detach();
        m_layers.erase(it);
    }
    //-------------------------------------------------------------------------
    void LayerStack::remove_layer(Layer* layer)
    {
        auto it = std::find_if(std::begin(m_layers), std::end(m_layers), [layer](const rtl::UniquePtr<Layer>& l)
                               {
                                   return layer == l.get();
                               });

        if (it == std::cend(m_layers))
            return;

        (*it)->detach();
        m_layers.erase(it);
    }

    //-------------------------------------------------------------------------
    void LayerStack::clear()
    {
        for (auto& l : m_layers)
        {
            l->detach();
        }

        m_layers.clear();
    }
}