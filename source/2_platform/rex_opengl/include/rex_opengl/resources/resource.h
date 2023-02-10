#pragma once

#include "rex_engine/types.h"

namespace rex
{
    class Resource
    {
    public:
        Resource() = default;
        virtual ~Resource() = default;

        virtual u32 get_resource_id() const = 0;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        virtual void release() = 0;
    };
}