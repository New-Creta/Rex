#include "rex_directx/scenegraph/mesh_visitor.h"
#include "rex_directx/scenegraph/mesh_node.h"

namespace rex
{
  MeshVisitor::MeshVisitor()  = default;
  MeshVisitor::~MeshVisitor() = default;

  void MeshVisitor::visit(AbstractSceneNode& sceneNode)
  {
    if(sceneNode.type() == rsl::type_id<MeshNode>().hash_code())
    {
      MeshNode& mesh_node = static_cast<MeshNode&>(sceneNode);

      mesh_node.draw();
    }
  }
} // namespace rex