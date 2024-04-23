#pragma once

#include "rex_engine/engine/defines.h"

namespace rex
{
  class AbstractSceneNode;

  class SceneVisitor
  {
  public:
    SceneVisitor() = default;
    virtual ~SceneVisitor() = default;

    virtual void visit(AbstractSceneNode& sceneNode)
    {
      UNUSED_PARAM(sceneNode);
    }
  };
}