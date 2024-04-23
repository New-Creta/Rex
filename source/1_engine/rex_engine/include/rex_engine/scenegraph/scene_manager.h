#pragma once

#include "rex_engine/engine/singleton.h"
#include "rex_engine/scenegraph/scene.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

namespace rex
{

  class SceneManagerImpl
  {
  public:
    SceneManagerImpl();
    ~SceneManagerImpl();

    Scene* create_scene(const rsl::string_view sceneName);

    Scene* active_scene();
    const Scene* active_scene() const;

    void set_active(const rsl::string_view sceneName);

  private:
    Scene* m_active_scene;
    rsl::unordered_map<rsl::string, Scene> m_scenes;
  };

  class SceneManager : public StaticSingleton<SceneManagerImpl>
  {
  };

} // namespace rex