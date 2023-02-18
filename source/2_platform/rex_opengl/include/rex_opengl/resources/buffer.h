#pragma once

#include "rex_engine/types.h"
#include "rex_opengl/resources/resource.h"

namespace rex
{
  enum class BufferUsage
  {
    STATIC_DRAW,
    DYNAMIC_DRAW
  };

  class Buffer : public Resource
  {
  public:
    Buffer(u32 target);
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;
    ~Buffer() override;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) noexcept;

    u32 get_resource_id() const override;
    u32 get_resource_target() const;

    void bind() const override;
    void unbind() const override;

    void release() override;

  private:
    u32 m_resource_id;
    u32 m_resource_target;
  };

  u32 get_gl_usage(const BufferUsage& usage);
} // namespace rex