#include "rex_engine/scenegraph/scene_node.h"
#include "rex_engine/scenegraph/scene_visitor.h"

namespace rex
{
  AbstractSceneNode::AbstractSceneNode(const glm::mat4& localTransform, const rsl::string& name)
      : m_name(name)
  {
    m_local_transform   = localTransform;
    m_inverse_transform = glm::inverse(localTransform);
  }

  AbstractSceneNode::~AbstractSceneNode() = default;

  void AbstractSceneNode::accept(SceneVisitor& visitor)
  {
    visitor.visit(*this);

    // Visit children
    for(auto& child: m_children)
    {
      child->accept(visitor);
    }
  }

  const rsl::string& AbstractSceneNode::get_name() const
  {
    return m_name;
  }

  void AbstractSceneNode::set_name(const rsl::string& name)
  {
    m_name = name;
  }

  const glm::mat4& AbstractSceneNode::get_local_transform() const
  {
    return m_local_transform;
  }

  void AbstractSceneNode::set_local_transform(const glm::mat4& localTransform)
  {
    m_local_transform   = localTransform;
    m_inverse_transform = glm::inverse(localTransform);
  }

  const glm::mat4& AbstractSceneNode::get_inverse_local_transform() const
  {
    return m_inverse_transform;
  }

  glm::mat4 AbstractSceneNode::get_world_transform() const
  {
    return m_local_transform * get_parent_world_transform();
  }

  glm::mat4 AbstractSceneNode::get_inverse_world_transform() const
  {
    return glm::inverse(get_world_transform());
  }

  glm::mat4 AbstractSceneNode::get_parent_world_transform() const
  {
    glm::mat4 parent_transform = glm::mat4(1.0f);

    if(auto parent_node = m_parent_node.lock())
    {
      parent_transform = parent_node->get_world_transform();
    }

    return parent_transform;
  }

  void AbstractSceneNode::add_child(rsl::shared_ptr<AbstractSceneNode> childNode)
  {
    if(childNode)
    {
      NodeList::iterator iter = rsl::find(m_children.begin(), m_children.end(), childNode);
      if(iter == m_children.end())
      {
        glm::mat4 world_transform = childNode->get_world_transform();
        childNode->m_parent_node          = shared_from_this();
        glm::mat4 local_transform = world_transform * get_inverse_world_transform();
        childNode->set_local_transform(local_transform);
        m_children.push_back(childNode);
        if(!childNode->get_name().empty())
        {
          m_children_by_name.emplace(childNode->get_name(), childNode);
        }
      }
    }
  }

  void AbstractSceneNode::remove_child(rsl::shared_ptr<AbstractSceneNode> childNode)
  {
    if(childNode)
    {
      NodeList::const_iterator iter = rsl::find(m_children.begin(), m_children.end(), childNode);
      if(iter != m_children.cend())
      {
        childNode->set_parent(nullptr);
        m_children.erase(iter);

        // Also remove it from the name map.
        NodeNameMap::iterator iter2 = m_children_by_name.find(childNode->get_name());
        if(iter2 != m_children_by_name.end())
        {
          m_children_by_name.erase(iter2);
        }
      }
      else
      {
        // Maybe the child appears deeper in the scene graph.
        for(auto child: m_children)
        {
          child->remove_child(childNode);
        }
      }
    }
  }

  void AbstractSceneNode::set_parent(rsl::shared_ptr<AbstractSceneNode> parentNode)
  {
    // Parents own their children.. If this node is not owned
    // by anyone else, it will cease to exist if we remove it from it's parent.
    // As a precaution, store myself as a shared pointer so I don't get deleted
    // half-way through this function!
    // Technically self deletion shouldn't occur because the thing invoking this function
    // should have a shared_ptr to it.
    rsl::shared_ptr<AbstractSceneNode> me = shared_from_this();
    if(parentNode)
    {
      parentNode->add_child(me);
    }
    else if(auto parent = m_parent_node.lock())
    {
      // Setting parent to NULL.. remove from current parent and reset parent node.
      auto world_transform = get_world_transform();
      parent->remove_child(me);
      m_parent_node.reset();
      set_local_transform(world_transform);
    }
  }
} // namespace rex