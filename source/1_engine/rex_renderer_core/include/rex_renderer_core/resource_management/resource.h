#pragma once

#include "rex_renderer_core/system/resource_hasher.h"
#include "rex_std/bonus/utility/type_id.h"

namespace rex
{
  namespace rhi
  {
    class IResource
    {
    public:
      IResource()          = default;
      virtual ~IResource() = default;

      virtual size_t type() const       = 0;
      virtual ResourceHash hash() const = 0;
    };

    template <typename T>
    class BaseResource : public IResource
    {
    public:
      explicit BaseResource(T* resource, ResourceHash hash)
          : m_resource(resource)
          , m_hash(hash)
      {
      }

      ~BaseResource() override = default;

      static size_t static_type()
      {
        return rsl::type_id<T>().hash_code();
      }
      size_t type() const override
      {
        return static_type();
      }

      T* get()
      {
        return m_resource;
      }
      const T* get() const
      {
        return m_resource;
      }

      ResourceHash hash() const override
      {
        return m_hash;
      }

    private:
      T* m_resource;
      ResourceHash m_hash;
    };
  } // namespace rhi
} // namespace rex