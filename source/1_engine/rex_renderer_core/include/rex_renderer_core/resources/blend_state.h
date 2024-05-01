#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

namespace rex
{
  namespace rhi
  {
    enum class Blend
    {
      Zero,
      One,
      SrcColor,
      InvSrcColor,
      SrcAlpha,
      InvSrcAlpha,
      DestAlpha,
      InvDestAlpha,
      DestColor,
      InvDestColor,
      SrcAlphaSat,
      BlendFactor,
      InvBlendFactor,
      Src1Color,
      InvSrc1Color,
      Src1Alpha,
      InvSrc1Alpha
    };

    enum class BlendOp
    {
      Add,
      Subtract,
      RevSubtract,
      Min,
      Max
    };

    enum class LogicOp
    {
      Clear,
      Set,
      Copy,
      CopyInverted,
      Noop,
      And,
      Nand,
      Or,
      Nor,
      Xor,
      Equiv,
      AndReverse,
      AndInverted,
      OrReverse,
      OrInverted
    };

    enum class RenderTargetWriteMask
    {
      Red = BIT(0),
      Green = BIT(1),
      Blue = BIT(2),
      Alpha = BIT(3),
      All = Red | Green | Blue | Alpha
    };

    struct RenderTargetBlendDesc
    {
      Blend src_blend;
      Blend dst_blend;
      BlendOp blend_op;
      Blend src_blend_alpha;
      Blend dst_blend_alpha;
      BlendOp blend_op_alpha;
      LogicOp logic_op;
      RenderTargetWriteMask render_target_write_mask;
      bool blend_enable;
      bool logic_op_enable;
    };

    struct BlendState
    {
      bool enable_alpha_to_coverage;
      bool independent_blend_state;
      RenderTargetBlendDesc render_target[8];
    };
  }
}