#pragma once

#include "resources/resource.h"

namespace rex
{
    class VertexArray : public Resource
    {
    public:
        VertexArray();
        ~VertexArray() override;

        u32 get_resource_id() const override;

        void bind() const override;
        void unbind() const override;

        void release() override;

    private:
        u32 m_resource_id;
    };
}