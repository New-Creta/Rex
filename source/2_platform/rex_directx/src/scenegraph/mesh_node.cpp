#include "rex_directx/scenegraph/mesh_node.h"
#include "rex_directx/scenegraph/camera_node.h"
#include "rex_engine/scenegraph/scene.h"
#include "rex_engine/scenegraph/scene_manager.h"
#include "rex_directx/resources/shader_library.h"
#include "rex_directx/mesh.h"

#include "rex_renderer_core/renderer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace rex
{
  MeshNode::MeshNode(const glm::mat4& localTransformation, const rsl::string& name)
      : SceneNode(localTransformation, name)
  {
  }

  void MeshNode::draw()
  {
    CameraNode* camera_node = SceneManager::instance()->active_scene()->root_node()->find_child<CameraNode>();

    glm::mat4 view  = camera_node->view();
    glm::mat4 proj  = camera_node->proj();
    glm::mat4 world = get_local_transform();
    glm::mat4 mvp   = proj * view * world;
    glm::mat4 transposed_mvp = glm::transpose(mvp);

    renderer::set_graphics_dynamic_constant_buffer(renderer::shader_library::root_parameters::Unlit::MATRICES_CB, transposed_mvp);

    for(rsl::shared_ptr<renderer::Mesh>& m: m_meshes)
    {
      m->draw();
    }
  }

  /**
   * Add a mesh to this scene node.
   * @returns The index of the mesh in the mesh list.
   */
  size_t MeshNode::add_mesh(rsl::shared_ptr<renderer::Mesh> mesh)
  {
    size_t index = (size_t)-1;
    if(mesh)
    {
      MeshList::const_iterator iter = rsl::find(m_meshes.begin(), m_meshes.end(), mesh);
      if(iter == m_meshes.cend())
      {
        index = m_meshes.size();
        m_meshes.push_back(mesh);
      }
      else
      {
        index = iter - m_meshes.begin();
      }
    }

    return index;
  }

  void MeshNode::remove_mesh(rsl::shared_ptr<renderer::Mesh> mesh)
  {
    if(mesh)
    {
      MeshList::const_iterator iter = rsl::find(m_meshes.begin(), m_meshes.end(), mesh);
      if(iter != m_meshes.end())
      {
        m_meshes.erase(iter);
      }
    }
  }
} // namespace rex