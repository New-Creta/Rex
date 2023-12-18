#pragma once

#define RESOURCE_CLASS_TYPE(resourceType)  static size_t           static_type()                { return typeid(resourceType).hash_code(); }    \
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
            BaseResource() = default;
            ~BaseResource() override = default;

            virtual T* get() = 0;
            virtual const T* get() const = 0;
        };
    }
}