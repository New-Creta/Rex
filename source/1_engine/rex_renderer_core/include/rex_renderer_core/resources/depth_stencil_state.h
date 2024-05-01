#pragma once

#include "rex_renderer_core/rendering/comparison_func.h"

namespace rex
{
  namespace rhi
  {
    enum class DepthWriteMask
    {
      DepthWriteMaskZero,
      DepthWriteMaskAll
    };

    enum class StencilOp
    {
      Keep,
      Zero,
      Replace,
      IncrSat,
      DecrSat,
      Invert,
      Incr,
      Decr
    };

    struct DepthStencilOpDesc
    {
      StencilOp stencil_fail_op;
      StencilOp stencil_depth_fail_op;
      StencilOp stencil_pass_op;
      renderer::ComparisonFunc stencil_func;
    };

    struct DepthStencilDesc
    {
      bool depth_enable;
      DepthWriteMask depth_write_mask;
      renderer::ComparisonFunc depth_func;
      bool stencil_enable;
      u8 stencil_read_mask;
      u8 stencil_write_mask;
      DepthStencilOpDesc front_face;
      DepthStencilOpDesc back_face;
    };
  }
}