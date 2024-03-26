#include "regina/cube_scene.h"

#include "rex_engine/primitives/mesh_factory.h"
#include "rex_engine/primitives/box.h"
#include "rex_renderer_core/resources/vertex.h"
#include "rex_renderer_core/commands/create_buffer_cmd.h"

#include "rex_engine/memory/blob_writer.h"

#include "rex_windows/app/gui_application.h"
#include "rex_engine/app/windowinfo.h"

#include <glm/gtc/matrix_transform.hpp>

namespace regina
{
  CubeScene::CubeScene()
  {
    build_geometry();
    build_render_items();

    build_shader("regina\\Shaders\\color.hlsl", "regina\\Shaders\\color.hlsl");
    build_input_layout();

    build_raster_state();
    build_pso();
    build_frame_resources();

    f32 width = rex::globals::window_info().width;
    f32 height = rex::globals::window_info().height;
    build_constant_buffers(width, height);

    use_pso();
  }

  void CubeScene::build_geometry()
  {
    // Create the box geometry
    auto box = rex::mesh_factory::create_box(1.5f, 1.5f, 1.5f, 0);

    // Convert the box geometry into known vertex type used by the renderer
    REX_STATIC_WARNING("Convert mesh geometry vertex into a renderer known vertex");
    auto total_vertex_count = box.vertices().size();
    const u32 vb_byte_size = total_vertex_count * sizeof(rex::renderer::VertexPosCol);

    rsl::unique_array<rex::renderer::VertexPosCol> box_vertices = rsl::make_unique<rex::renderer::VertexPosCol[]>((total_vertex_count));
    for (s32 idx = 0; idx < box.vertices().size(); ++idx)
    {
      const rex::mesh_factory::Vertex& v = box.vertices()[idx];
      const rex::renderer::VertexPosCol nv({ v.position.x, v.position.y, v.position.z }, { v.position.x, v.position.y, v.position.z, 1.0f });
      box_vertices[idx] = nv;
    }

    // Copy the indices into an index buffer blob
    rex::memory::Blob index_buffer(rsl::make_unique<u16[]>(box.indices().size()));
    const u32 ib_byte_size = box.indices().size() * sizeof(u16);
    index_buffer.write(box.indices().data(), rsl::memory_size(ib_byte_size));

    // Fill in the constant buffer
    glm::mat4 world = glm::mat4(1.0f);
    world = glm::scale(world, glm::vec3(2.0f, 2.0f, 2.0f));
    rex::memory::Blob constant_buffer(rsl::make_unique<rsl::byte[]>(sizeof(world)));
    constant_buffer.write(&world, rsl::memory_size(sizeof(world)));
    
    // Fill in the buffer descs to transfer the data over to mesh
    rex::renderer::VertexBufferDesc vb_desc(rsl::move(box_vertices));
    rex::renderer::IndexBufferDesc ib_desc{rsl::move(index_buffer), rex::renderer::IndexBufferFormat::Uint16, box.indices().size()};
    rex::renderer::ConstantBufferDesc cb_desc{ rsl::move(constant_buffer) };

    // Create the cube mesh object
    m_mesh_cube = rsl::make_unique<rex::renderer::Mesh>("box_geometry"_med, rsl::move(vb_desc), rsl::move(ib_desc), rsl::move(cb_desc));

    // Meshes can have multiple submeshes.
    // In this case the submesh points to the entire mesh, therefore we configure it as such
    s32 start_idx = 0;
    s32 last_idx = box.indices().size();
    m_mesh_cube->add_submesh("box"_small, start_idx, last_idx);

    // Pass the mesh to the renderer so it'll render it next frame
    rex::renderer::add_mesh(m_mesh_cube.get());
  }

  void CubeScene::build_render_items()
  {
    //// This scene only holds 1 mesh, which is a cube so we simply add that to the renderer
    //renderer::add_mesh(m_mesh_cube.get());


    //auto cube_r_item = rex::renderer::RenderItem();

    //const glm::mat4 scale = glm::scale(cube_r_item.world, glm::vec3(2.0f, 2.0f, 2.0f));

    //cube_r_item.world = scale;
    //cube_r_item.constant_buffer_index = 0;
    //cube_r_item.geometry = m_mesh_cube.get();
    //cube_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
    //cube_r_item.index_count = cube_r_item.geometry->submesh("box"_small)->index_count;
    //cube_r_item.start_index_location = cube_r_item.geometry->submesh("box"_small)->start_index_location;
    //cube_r_item.base_vertex_location = cube_r_item.geometry->submesh("box"_small)->base_vertex_location;

    //// Dirty flag indicating the object data has changed and we need to update the constant buffer.
    //// Because we have an object CBuffer for each FrameResource, we have to apply the
    //// update to each FrameResource.
    ////
    //// Thus, when we modify object data we should set NumFramesDirty = gNumFrameResources
    //// so that each frame resource gets the update.
    //cube_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

    //add_render_item(rsl::move(cube_r_item));
  }
}