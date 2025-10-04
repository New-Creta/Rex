#pragma once

#include "rex_std/vector.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  // Removes an element from src at given and index and adds it to the end of dst
  template <typename T>
  T& transfer_object_between_vectors(s32 idx, rsl::vector<T>& src, rsl::vector<T>& dst)
  {
    auto ctx = rsl::move(src[idx]);
    auto it = src.begin() + idx;
    src.erase(it);
    dst.emplace_back(rsl::move(ctx));
    return dst.back();
  }

  // This adds as an indirection for an element in a container
  // This is useful for container that store objects directly and not pointer
  // as this allows a user to always be able to access that object
  // even after a reallocation has taken place
  template <typename Container, typename Accessor>
  class ContainerElement
  {
  public:
    ContainerElement(Container& container, const Accessor& accessor)
      : m_container(&container)
      , m_accessor(accessor)
    {}

    auto operator->()
    {
      return &value()
    }
    auto operator->() const
    {
      return &value()
    }
    auto operator*()
    {
      return value();
    }
    auto operator*() const
    {
      return value();
    }

  private:
    auto value()
    {
      Container& container_ref = *m_container;
      return container_ref[m_accessor];
    }

  private:
    Container* m_container;
    Accessor m_accessor;
  };
}