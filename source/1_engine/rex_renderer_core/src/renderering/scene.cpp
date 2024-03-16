#include "rex_renderer_core/rendering/scene.h"

#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/commands/update_committed_resource_cmd.h"

namespace rex
{
  namespace renderer
  {
    struct ObjectConstants
    {
      glm::mat4 world = glm::mat4(1.0f);
    };

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

    void Scene::render(rex::renderer::SceneRenderer* renderer)
    {
      // Nothing to implement
    }

    //-------------------------------------------------------------------------
    u32 Scene::render_item_count() const
    {
      return m_render_items.size();
    }

    void Scene::update_object_constant_buffers(const rex::renderer::ResourceSlot& currObjectCommittedResource)
    {
      for (const auto& render_item : m_render_items)
      {
        // Only update the CBuffer data if the constants have changed.
        // This needs to be tracked per frame resource.
        if (render_item.num_frames_dirty > 0)
        {
          // Assign the new world matrixz
          ObjectConstants obj_constants;
          obj_constants.world = glm::transpose(render_item.world); // DirectX backend ( so we have to transpose, expects row major matrices )

          rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
          update_constant_buffer_command_desc.element_index = render_item.constant_buffer_index;
          update_constant_buffer_command_desc.buffer_data = rex::memory::make_blob(reinterpret_cast<rsl::byte*>(&obj_constants), rsl::memory_size(sizeof(ObjectConstants)));

          rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), currObjectCommittedResource);
        }
      }
    }
    void Scene::render_items(SceneRenderer* renderer, const rsl::vector<ResourceSlot>& contantBuffers) const
    {
      for (const auto& render_item : m_render_items)
      {
        rex::renderer::set_vertex_buffer(render_item.geometry->vertex_buffer_slot(), 0, render_item.geometry->vertex_buffer_byte_stride(), 0);
        rex::renderer::set_index_buffer(render_item.geometry->index_buffer_slot(), render_item.geometry->index_buffer_format(), 0);
        rex::renderer::set_primitive_topology(rex::renderer::PrimitiveTopology::TRIANGLELIST);

        rex::renderer::ResourceSlot const curr_object_cb = contantBuffers[render_item.constant_buffer_index];
        rex::renderer::set_constant_buffer_view(curr_object_cb, 0);

        rex::renderer::renderer_draw_indexed_instanced(1, 0, render_item.index_count, render_item.start_index_location, render_item.base_vertex_location);
      }

    }

  } // namespace renderer
} // namespace rex