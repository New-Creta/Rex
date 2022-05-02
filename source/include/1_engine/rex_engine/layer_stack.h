#pragma once

#include "rex_stl/containers/vector.h"

namespace rex
{
    class Layer;

    class LayerStack
    {
    public:
        using LayerStackIt = rtl::vector<rtl::UniquePtr<Layer>>::iterator;
        using ConstLayerStackIt = rtl::vector<rtl::UniquePtr<Layer>>::const_iterator;
        using ReverseLayerStackIt = rtl::vector<rtl::UniquePtr<Layer>>::reverse_iterator;
        using ConstReverseLayerStackIt = rtl::vector<rtl::UniquePtr<Layer>>::const_reverse_iterator;

        LayerStack(int32 initialSize = -1);
        ~LayerStack();

        //
        // Stack iterator
        //
        LayerStackIt begin();
        ConstLayerStackIt begin() const;
        ConstLayerStackIt cbegin() const;
        LayerStackIt end();
        ConstLayerStackIt end() const;
        ConstLayerStackIt cend() const;

        ReverseLayerStackIt rbegin();
        ConstReverseLayerStackIt crbegin() const;
        ReverseLayerStackIt rend();
        ConstReverseLayerStackIt crend() const;

        //
        // Stack functionality
        //
        void push(rtl::UniquePtr<Layer> layer);

        void remove_layer(const std::string& layerName);
        void remove_layer(Layer* layer);

        void clear();

    private:
        rtl::vector<rtl::UniquePtr<Layer>> m_layers;
    };
}