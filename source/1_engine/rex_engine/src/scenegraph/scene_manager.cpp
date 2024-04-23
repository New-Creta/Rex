#include "rex_engine/scenegraph/scene_manager.h"

namespace rex
{
  SceneManagerImpl::SceneManagerImpl()
  {
    m_active_scene = create_scene("sample");
  }
  SceneManagerImpl::~SceneManagerImpl() = default;

  Scene* SceneManagerImpl::create_scene(const rsl::string_view sceneName)
  {
    m_scenes[sceneName] = Scene {};
    return &m_scenes[sceneName];
  }

  Scene* SceneManagerImpl::active_scene()
  {
    return m_active_scene;
  }
  const Scene* SceneManagerImpl::active_scene() const
  {
    return m_active_scene;
  }

  void SceneManagerImpl::set_active(const rsl::string_view sceneName)
  {
    auto it = m_scenes.find(sceneName);
    if(it != rsl::cend(m_scenes))
    {
      m_active_scene = &(it->value);
    }
  }
} // namespace rex