#pragma once

namespace rex
{
  namespace rhi
  {
    class VertexBuffer;
    class IndexBuffer;
  }

  namespace d3d
  {
    template <typename T>
    struct ResourceTraits
    {
      using d3d_type = void;
    };

    struct ResourceTraits<rhi::VertexBuffer>
    {
      using d3d_type = rhi::VertexBuffer;
    };

    struct ResourceTraits<rhi::IndexBuffer>
    {
      using d3d_type = rhi::IndexBuffer;
    };
  }
}