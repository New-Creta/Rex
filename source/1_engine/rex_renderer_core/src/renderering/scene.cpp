#include "rex_renderer_core/rendering/scene.h"

namespace rex
{
    namespace renderer
    {
        //-------------------------------------------------------------------------
        void Scene::add_render_item(RenderItem&& item)
        {
            m_render_items.push_back(rsl::move(item));
        }
        //-------------------------------------------------------------------------
        void Scene::clear_render_items()
        {
            m_render_items.clear();
        }

        //-------------------------------------------------------------------------
        u32 Scene::render_item_count() const
        {
            return m_render_items.size();
        }

        //-------------------------------------------------------------------------
        Scene::RenderItemIt Scene::begin()
        {
            return m_render_items.begin();
        }
        //-------------------------------------------------------------------------
        Scene::ConstRenderItemIt Scene::cbegin() const 
        {
            return m_render_items.cbegin();
        }
        //-------------------------------------------------------------------------
        Scene::RenderItemIt Scene::end() 
        {
            return m_render_items.end();
        }
        //-------------------------------------------------------------------------
        Scene::ConstRenderItemIt Scene::cend() const
        {
            return m_render_items.cend();
        }
    }
}