#pragma once

#include "rex_renderer_core/rendering/scene.h"
#include "rex_renderer_core/resources/mesh.h"

#include "rex_std/unordered_map.h"
#include "rex_std/memory.h"
#include "rex_std/bonus/string.h"

#include "rex_renderer_core/resource_management/resource.h"
#include "rex_renderer_core/resources/vertex.h"
#include "rex_engine/primitives/mesh_factory.h"

namespace regina
{
  class SampleScene : public rex::renderer::Scene
  {
  public:
    SampleScene();

  protected:
    void update_object_constant_buffers() override;

  private:
    void build_geometry();

    rex::renderer::Submesh build_submesh(const rex::mesh_factory::MeshData16& meshData, rsl::vector<rex::renderer::VertexPosCol>& vertices, rsl::vector<u16>& indices);

  private:
    rsl::unique_ptr<rex::renderer::Mesh> m_geometry;
    rex::memory::Blob m_vb;
    rex::memory::Blob m_ib;

    glm::mat4 m_cube_world;;
    rex::renderer::RenderItem* m_cube_render_item;

  };
}