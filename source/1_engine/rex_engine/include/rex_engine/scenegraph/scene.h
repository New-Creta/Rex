#pragma once

#include "rex_std/memory.h"

namespace rex
{
  class SceneNode;
  class SceneVisitor;

  class Scene
  {
  public:
    Scene();
    ~Scene();

    void set_root_node(rsl::shared_ptr<SceneNode>&& sceneNode);
    void accept(SceneVisitor& visitor);

    SceneNode* root_node();
    const SceneNode* root_node() const;

  private:
    rsl::shared_ptr<SceneNode> m_root_node;
  };
} // namespace cera