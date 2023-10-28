#pragma once

namespace rex
{
    namespace renderer
    {
        class IResource
        {
        public:
            IResource() = default;
            virtual ~IResource() = default;
        };

        template<typename T>
        class BaseResource : public IResource
        {
        public:
            BaseResource() = default;
            ~BaseResource() override = default;

            virtual const T* get() const = 0;
        };
    }
}