#pragma once

#include "rex_renderer_core/rendering/render_item.h"

#include <rex_std/vector.h>

namespace rex
{
    namespace renderer
    {
        class Scene
        {
        public:
            using RenderItems = rsl::vector<RenderItem>;
            using RenderItemIt = RenderItems::iterator;
            using ConstRenderItemIt = RenderItems::const_iterator;

            void                add_render_item(RenderItem&& item);
            void                clear_render_items();

            u32                 render_item_count() const;

            RenderItemIt        begin();
            ConstRenderItemIt   cbegin() const;
            RenderItemIt        end();
            ConstRenderItemIt   cend() const;

        private:
            RenderItems         m_render_items;
        };
    }
}