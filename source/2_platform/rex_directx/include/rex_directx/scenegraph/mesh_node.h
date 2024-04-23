#pragma once

#include "rex_engine/scenegraph/scene_node.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace renderer
  {
    class Mesh;
  } // namespace renderer

  class MeshNode : public SceneNode
  {
  public:
    SCENE_NODE_CLASS_TYPE(MeshNode)

    MeshNode(const glm::mat4& localTransformation = glm::mat4(1.0), const rsl::string& name = rsl::string("MeshNode"));

    void draw();

    /**
     * Add a mesh to this scene node.
     * @returns The index of the mesh in the mesh list.
     */
    size_t add_mesh(rsl::shared_ptr<renderer::Mesh> mesh);
    void remove_mesh(rsl::shared_ptr<renderer::Mesh> mesh);

  private:
    using MeshList = rsl::vector<rsl::shared_ptr<renderer::Mesh>>;

    MeshList m_meshes;
  };
} // namespace rex