#pragma once

#include "rex_std/bonus/utility/type_id.h"

namespace rex
{
  namespace rhi
  {
    class IResource
    {
    public:
      IResource() = default;
      virtual ~IResource() = default;

      virtual size_t type() const = 0;
    };

    template <typename T>
    class BaseResource : public IResource
    {
    public:
      explicit BaseResource(T* resource)
        : m_resource(resource)
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

    private:
      T* m_resource;
    };
  } // namespace renderer
} // namespace rex