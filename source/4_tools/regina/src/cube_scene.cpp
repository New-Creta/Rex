#include "regina/cube_scene.h"

#include "rex_engine/primitives/mesh_factory.h"
#include "rex_engine/primitives/box.h"
#include "rex_renderer_core/resources/vertex.h"

#include "rex_engine/memory/blob_writer.h"

#include "rex_windows/app/gui_application.h"
#include "rex_engine/app/windowinfo.h"

#include <glm/gtc/matrix_transform.hpp>

#include "rex_renderer_core/system/rhi.h"

namespace regina
{
  CubeScene::CubeScene()
  {
    build_geometry();
   
    build_shader("regina\\Shaders\\color.hlsl", "regina\\Shaders\\color.hlsl");
    build_input_layout();

    build_raster_state();
    build_pso();

    f32 width = rex::globals::window_info().width;
    f32 height = rex::globals::window_info().height;
    build_constant_buffers(width, height);

    use_pso();

    m_cube_world = glm::mat4(1.0f);
  }

  void CubeScene::update_object_constant_buffers()
  {
    //m_cube_world = glm::rotate(m_cube_world, 3.14f / 400, glm::vec3(01.0f, 1.0f, 0.0f));
    rex::rhi::update_buffer(m_cube_render_item->cb(), &m_cube_world, sizeof(m_cube_world));
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
      const rex::renderer::VertexPosCol nv({ v.position.x, v.position.y, v.position.z }, { v.normal.x, v.normal.y, v.normal.z, 1.0f });
      box_vertices[idx] = nv;
    }

    // Copy the indices into an index buffer blob
    rex::memory::Blob index_buffer(rsl::make_unique<u16[]>(box.indices().size()));
    const u32 ib_byte_size = box.indices().size() * sizeof(u16);
    index_buffer.write(box.indices().data(), rsl::memory_size(ib_byte_size));

    // Fill in the constant buffer
    m_cube_world = glm::scale(m_cube_world, glm::vec3(2.0f, 2.0f, 2.0f));
    //m_cube_world = glm::rotate(m_cube_world, 3.14f / 4, glm::vec3(0.0f, 1.0f, 0.0f));
    rex::memory::Blob constant_buffer(rsl::make_unique<rsl::byte[]>(sizeof(m_cube_world)));
    constant_buffer.write(&m_cube_world, rsl::memory_size(sizeof(m_cube_world)));
    
    // Fill in the buffer descs to transfer the data over to mesh
    rex::memory::Blob vb_data(rsl::move(box_vertices));
    rex::memory::Blob ib_data(rsl::move(index_buffer));

    rex::rhi::VertexBufferDesc vb_desc{ rex::memory::BlobView(vb_data), sizeof(rex::renderer::VertexPosCol) };
    rex::rhi::IndexBufferDesc ib_desc{rex::memory::BlobView(ib_data), rex::renderer::IndexBufferFormat::Uint16, box.indices().size()};
    rex::rhi::ConstantBufferDesc cb_desc{ rex::memory::BlobView(constant_buffer) };

    // Create the cube mesh object
    m_mesh_cube = rsl::make_unique<rex::renderer::Mesh>("box_geometry"_med, vb_desc, ib_desc);

    // Meshes can have multiple submeshes.
    // In this case the submesh points to the entire mesh, therefore we configure it as such
    rex::renderer::Submesh submesh{};
    submesh.base_vertex_location = 0;
    submesh.start_index_location = 0;
    submesh.index_count = box.indices().size();
    m_mesh_cube->add_submesh("box"_small, submesh);

    // Pass the mesh to the renderer so it'll render it next frame
    m_cube_render_item = rex::renderer::add_mesh(m_mesh_cube.get(), submesh, rex::memory::BlobView(constant_buffer));
  }
}