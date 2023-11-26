#include "rex_directx/rendering/scene.h"

namespace rex
{
    namespace renderer
    {
        //-------------------------------------------------------------------------
        void Scene::add_render_item(RenderItem&& item)
        {
            m_render_items.push_back(std::move(item));
        }
        //-------------------------------------------------------------------------
        void Scene::clear_render_items()
        {
            m_render_items.clear();
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