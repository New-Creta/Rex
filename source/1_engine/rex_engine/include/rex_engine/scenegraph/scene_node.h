#pragma once

#include "rex_std/bonus/utility/type_id.h"
#include "rex_std/map.h"
#include "rex_std/memory.h"
#include "rex_std/string.h"
#include "rex_std/vector.h"

#include <glm/glm.hpp>

#define SCENE_NODE_CLASS_TYPE(resourceType)                                                                                                                                                                                                              \
  static size_t static_type()                                                                                                                                                                                                                            \
  {                                                                                                                                                                                                                                                      \
    return rsl::type_id<resourceType>().hash_code();                                                                                                                                                                                                     \
  }                                                                                                                                                                                                                                                      \
  size_t type() const override                                                                                                                                                                                                                           \
  {                                                                                                                                                                                                                                                      \
    return static_type();                                                                                                                                                                                                                                \
  }

namespace rex
{
  class SceneVisitor;

  class AbstractSceneNode : public rsl::enable_shared_from_this<AbstractSceneNode>
  {
  public:
    explicit AbstractSceneNode(const glm::mat4& localTransform = glm::mat4(1.0f), const rsl::string& name = rsl::string("AbstractSceneNode"));
    virtual ~AbstractSceneNode();

    void accept(SceneVisitor& visitor);

    /**
     * Assign a name to the scene node so it can be searched for later.
     */
    const rsl::string& get_name() const;
    void set_name(const rsl::string& name);

    /**
     * Get the scene nodes local (relative to its parent's transform).
     */
    const glm::mat4& get_local_transform() const;
    void set_local_transform(const glm::mat4& localTransform);

    /**
     * Get the inverse of the local transform.
     */
    const glm::mat4& get_inverse_local_transform() const;

    /**
     * Get the scene node's world transform (concatenated with its parents
     * world transform).
     */
    glm::mat4 get_world_transform() const;

    /**
     * Get the inverse of the world transform (concatenated with its parent's
     * world transform).
     */
    glm::mat4 get_inverse_world_transform() const;

    /**
     * Add a child node to this scene node.
     * NOTE: Circular references are not checked.
     * A scene node "owns" its children. If the root node
     * is deleted, all of its children are deleted if nothing
     * else is referencing them.
     */
    void add_child(rsl::shared_ptr<AbstractSceneNode> childNode);
    void remove_child(rsl::shared_ptr<AbstractSceneNode> childNode);
    void set_parent(rsl::shared_ptr<AbstractSceneNode> parentNode);

    /**
     * Type of the scene node
     */
    virtual size_t type() const = 0;

    /**
     * Type of the scene node
     */
    template <typename T>
    T* find_child();
    template <typename T>
    const T* find_child() const;

  private:
    glm::mat4 get_parent_world_transform() const;

  private:
    using NodePtr     = rsl::shared_ptr<AbstractSceneNode>;
    using NodeList    = rsl::vector<NodePtr>;
    using NodeNameMap = rsl::multimap<rsl::string, NodePtr>;

    glm::mat4 m_local_transform;
    glm::mat4 m_inverse_transform;

    rsl::string m_name;

    rsl::weak_ptr<AbstractSceneNode> m_parent_node;

    NodeList m_children;
    NodeNameMap m_children_by_name;
  };

  template <typename T>
  T* AbstractSceneNode::find_child()
  {
    if(type() == rsl::type_id<T>().hash_code())
    {
      return static_cast<T*>(this);
    }
    else if(m_children.empty() == false)
    {
      for(auto& c: m_children)
      {
        T* f = c->find_child<T>();
        if(f != nullptr)
        {
          return static_cast<T*>(f);
        }
      }
    }

    return nullptr;
  }

  template <typename T>
  const T* AbstractSceneNode::find_child() const
  {
    if(type() == rsl::type_id<T>().hash_code())
    {
      return static_cast<const T*>(this);
    }
    else if(m_children.empty() == false)
    {
      for(const auto& c: m_children)
      {
        const T* f = c->find_child<T>();
        if(f != nullptr)
        {
          return static_cast<const T*>(f);
        }
      }
    }

    return nullptr;
  }

  /**
   * An empty container for a SceneNode
   */
  class SceneNode : public AbstractSceneNode
  {
  public:
    SCENE_NODE_CLASS_TYPE(SceneNode)

    SceneNode(const glm::mat4& localTransformation = glm::mat4(1.0), const rsl::string& name = rsl::string("SceneNode"))
        : AbstractSceneNode(localTransformation, name)
    {
    }
  };
} // namespace rex