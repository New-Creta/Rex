#pragma once

namespace rex
{
  namespace gfx
  {
    // Graphics api abstraction of vertex buffer format
    enum class VertexBufferFormat
    {
      None = 0,
      Uint1,
      Uint2,
      Uint16_2,
      Uint3,
      Uint4,
      Int1,
      Int2,
      Int3,
      Int4,
      Float1,
      Float2,
      Float3,
      Float4,
      UNorm4,
      UNorm2,
      UNorm1
    };

    constexpr s32 format_byte_size(VertexBufferFormat format)
    {
      switch (format)
      {
      case rex::gfx::VertexBufferFormat::Uint1:     return 1 * sizeof(u32);
      case rex::gfx::VertexBufferFormat::Uint2:     return 2 * sizeof(u32);
      case rex::gfx::VertexBufferFormat::Uint16_2:  return 2 * sizeof(u16);
      case rex::gfx::VertexBufferFormat::Uint3:     return 3 * sizeof(u32);
      case rex::gfx::VertexBufferFormat::Uint4:     return 4 * sizeof(u32);
      case rex::gfx::VertexBufferFormat::Int1:      return 1 * sizeof(s32);
      case rex::gfx::VertexBufferFormat::Int2:      return 2 * sizeof(s32);
      case rex::gfx::VertexBufferFormat::Int3:      return 3 * sizeof(s32);
      case rex::gfx::VertexBufferFormat::Int4:      return 4 * sizeof(s32);
      case rex::gfx::VertexBufferFormat::Float1:    return 1 * sizeof(f32);
      case rex::gfx::VertexBufferFormat::Float2:    return 2 * sizeof(f32);
      case rex::gfx::VertexBufferFormat::Float3:    return 3 * sizeof(f32);
      case rex::gfx::VertexBufferFormat::Float4:    return 4 * sizeof(f32);
      case rex::gfx::VertexBufferFormat::UNorm4:    return 4 * sizeof(u8);
      case rex::gfx::VertexBufferFormat::UNorm2:    return 2 * sizeof(u8);
      case rex::gfx::VertexBufferFormat::UNorm1:    return 1 * sizeof(u8);
      }

      return invalid_obj<s32>();
    }
  } // namespace gfx
} // namespace rex