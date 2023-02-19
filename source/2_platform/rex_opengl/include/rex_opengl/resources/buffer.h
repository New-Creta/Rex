#pragma once

#include "rex_engine/types.h"
#include "rex_opengl/resources/resource.h"

namespace rex
{
  enum class BufferUsage
  {
    StaticDraw,
    DynamicDraw
  };

  class Buffer : public Resource
  {
  public:
    explicit Buffer(u32 target);
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) noexcept;
    ~Buffer() override;

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) noexcept;

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