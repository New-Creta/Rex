#pragma once

#include "rex_engine/scenegraph/scene_visitor.h"

namespace rex
{
  namespace renderer
  {
    class ICommandList;
  }
  class SceneNode;

  class MeshVisitor : public SceneVisitor
  {
  public:
    MeshVisitor();
    ~MeshVisitor() override;

    void visit(AbstractSceneNode& sceneNode) override;
  };
} // namespace rex