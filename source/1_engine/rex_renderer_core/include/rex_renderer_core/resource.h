#pragma once

#include "rex_std/bonus/utility/type_id.h"

#define RESOURCE_CLASS_TYPE(resourceType)  static size_t           static_type()                { return rsl::type_id<resourceType>().hash_code(); }    \
								                  size_t           type() const override        { return static_type(); }                       \

namespace rex
{
    namespace renderer
    {
        class IResource
        {
        public:
            IResource() = default;
            virtual ~IResource() = default;

            virtual size_t type() const = 0;
        };

        template<typename T>
        class BaseResource : public IResource
        {
        public:
          BaseResource(T* resource)
            : m_resource(resource)
          {}

            ~BaseResource() override = default;

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
    }
}