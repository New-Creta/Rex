#include "rex_engine/scenegraph/scene.h"

#include "rex_engine/scenegraph/scene_node.h"
#include "rex_engine/scenegraph/scene_visitor.h"

namespace rex
{
  Scene::Scene()
      : m_root_node(nullptr)
  {
  }

  Scene::~Scene() = default;

  void Scene::set_root_node(rsl::shared_ptr<SceneNode>&& root)
  {
    m_root_node = rsl::move(root);
  }

  SceneNode* Scene::root_node()
  {
    return m_root_node.get();
  }

  const SceneNode* Scene::root_node() const
  {
    return m_root_node.get();
  }

  void Scene::accept(SceneVisitor& visitor) 
  {
    if(m_root_node)
    {
      m_root_node->accept(visitor);
    }
  }

} // namespace rex