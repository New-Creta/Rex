#pragma once

namespace rex
{
    namespace engine
    {
        class Layer;

        class LayerStack
        {
        public:
            using LayerStackIt = std::vector<std::unique_ptr<Layer>>::iterator;
            using ConstLayerStackIt = std::vector<std::unique_ptr<Layer>>::const_iterator;
            using ReverseLayerStackIt = std::vector<std::unique_ptr<Layer>>::reverse_iterator;
            using ConstReverseLayerStackIt = std::vector<std::unique_ptr<Layer>>::const_reverse_iterator;

            LayerStack();
            ~LayerStack();

            //
            // Stack iterator
            //
            inline LayerStackIt                 begin()             { return m_layers.begin(); }
            inline ConstLayerStackIt            cbegin() const      { return m_layers.cbegin(); }
            inline LayerStackIt                 end()               { return m_layers.end(); }
            inline ConstLayerStackIt            cend() const        { return m_layers.cend(); }

            inline ReverseLayerStackIt          tbegin()            { return m_layers.rbegin(); }
            inline ConstReverseLayerStackIt     crbegin() const     { return m_layers.crbegin(); }
            inline ReverseLayerStackIt          rend()              { return m_layers.rend(); }
            inline ConstReverseLayerStackIt     crend() const       { return m_layers.crend(); }

            //
            // Stack functionality
            //
            void pushBack(std::unique_ptr<Layer> layer);
            void pushFront(std::unique_ptr<Layer> layer);
            void insert(std::unique_ptr<Layer> layer, int index);

            void removeLayer(const std::string& layer);
            void removeLayer(Layer* layer);

            void removeAll();

        private:
            std::vector<std::unique_ptr<Layer>> m_layers;
        };
    }
}