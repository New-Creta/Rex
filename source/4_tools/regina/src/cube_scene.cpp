#include "regina/cube_scene.h"

#include "rex_engine/primitives/mesh_factory.h"
#include "rex_engine/primitives/box.h"
#include "rex_renderer_core/resources/vertex.h"
#include "rex_renderer_core/commands/create_buffer_cmd.h"

#include <glm/gtc/matrix_transform.hpp>

namespace regina
{
  CubeScene::CubeScene()
  {
    build_geometry();
    build_render_items();
  }

  void CubeScene::build_geometry()
  {
    auto box = rex::mesh_factory::create_box(1.5f, 0.5f, 1.5f, 0);

    auto total_vertex_count = box.vertices().size();
    auto total_index_count = box.indices().size();

    rsl::vector<rex::renderer::VertexPosCol> box_vertices((rsl::Capacity(total_vertex_count)));
    for (const rex::mesh_factory::Vertex& v : box.vertices())
    {
      rex::renderer::VertexPosCol const nv({ v.position.x, v.position.y, v.position.z }, { v.position.x, v.position.y, v.position.z, 1.0f });
      box_vertices.push_back(nv);
    }

    rsl::vector<u16> box_indices((rsl::Capacity(total_index_count)));
    box_indices.insert(box_indices.end(), rsl::begin(box.indices()), rsl::end(box.indices()));

    const u32 vb_byte_size = total_vertex_count * sizeof(rex::renderer::VertexPosCol);
    const u32 ib_byte_size = total_index_count * sizeof(u16);

    rex::renderer::commands::CreateBufferCommandDesc v_create_buffer_command_desc = rex::renderer::commands::create_buffer_parameters<rex::renderer::VertexPosCol>(box_vertices.data(), box_vertices.size());
    rex::renderer::Mesh::VertexBufferDesc vbd(rex::renderer::create_vertex_buffer(rsl::move(v_create_buffer_command_desc)), sizeof(rex::renderer::VertexPosCol), vb_byte_size);

    rex::renderer::commands::CreateBufferCommandDesc i_create_buffer_command_desc = rex::renderer::commands::create_buffer_parameters<u16>(box_indices.data(), box_indices.size());
    rex::renderer::Mesh::IndexBufferDesc ibd(rex::renderer::create_index_buffer(rsl::move(i_create_buffer_command_desc)), rex::renderer::IndexBufferFormat::R16Uint, ib_byte_size);

    m_mesh_cube = rsl::make_unique<rex::renderer::Mesh>("box_geometry"_med, vbd, ibd);
    m_mesh_cube->add_submesh("box"_small, total_index_count, 0, 0);
  }

  void CubeScene::build_render_items()
  {
    auto cube_r_item = rex::renderer::RenderItem();

    const glm::mat4 scale = glm::scale(cube_r_item.world, glm::vec3(2.0f, 2.0f, 2.0f));

    cube_r_item.world = scale;
    cube_r_item.constant_buffer_index = 0;
    cube_r_item.geometry = m_mesh_cube.get();
    cube_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
    cube_r_item.index_count = cube_r_item.geometry->submesh("box"_small)->index_count;
    cube_r_item.start_index_location = cube_r_item.geometry->submesh("box"_small)->start_index_location;
    cube_r_item.base_vertex_location = cube_r_item.geometry->submesh("box"_small)->base_vertex_location;

    // Dirty flag indicating the object data has changed and we need to update the constant buffer.
    // Because we have an object CBuffer for each FrameResource, we have to apply the
    // update to each FrameResource.
    //
    // Thus, when we modify object data we should set NumFramesDirty = gNumFrameResources
    // so that each frame resource gets the update.
    cube_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

    add_render_item(rsl::move(cube_r_item));
  }
}