#pragma once

#include "rex_engine/types.h"
#include "rex_opengl/resources/resource.h"

namespace rex 
{
  class VertexArray : public Resource
  {
  public:
    VertexArray();
    VertexArray(const VertexArray& other) = delete;
    VertexArray(VertexArray&& other) noexcept;
    ~VertexArray() override;

    VertexArray& operator=(const VertexArray& other) = delete;
    VertexArray& operator=(VertexArray&& other) noexcept;

    u32 resource_id() const override;

    void bind() const override;
    void unbind() const override;

    void release() override;

  private:
    u32 m_resource_id;
  };
} // namespace rex 