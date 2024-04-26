#pragma once

#include "rex_std/bonus/utility/type_id.h"

#include "rex_renderer_core/system/resource_hasher.h"

namespace rex
{
  namespace rhi
  {
    // A resource can be almost any object that can be allocated on the gpu.
    // buffers, shaders, states, .. are all resources.

    // The resource interface.
    // A resource should technically only provide a way to get its type and its id
    // The type is used for runtime analysis, making sure we're operating on the right type of resources
    // A resource's ID is used to identify resources. Each resource id is unique for that resource
    class IResource
    {
    public:
      IResource() = default;
      virtual ~IResource() = default;

      virtual size_t type() const = 0;
      virtual ResourceID id() const = 0;
    };

    template <typename T>
    class BaseResource : public IResource
    {
    public:
      explicit BaseResource(T* resource, ResourceID id)
        : m_resource(resource)
        , m_id(id)
      {
      }

      ~BaseResource() override = default;

      // A resource's type is determined by the type id hash of its template type
      static size_t static_type()
      {
        return rsl::type_id<T>().hash_code();
      }
      // the runtime time, is just the static type, determined by its template type
      size_t type() const final
      {
        return static_type();
      }

      // return the wrapped resource type
      T* get()
      {
        return m_resource;
      }
      // return the wrapped resource type
      const T* get() const
      {
        return m_resource;
      }

      // return the id of this resource
      ResourceID id() const override
      {
        return m_id;
      }

    private:
      // We don't own the resource ourselves, as it's possible we need special delete logic for the resource.
      // Therefore the derived class is supposed to manage its wrapped resource's lifetime and cleanup
      // The base class just holds a non-owning pointer to this resource.
      T* m_resource; 

      // The GUID of this resource
      ResourceID m_id;
    };
  } // namespace renderer
} // namespace rex