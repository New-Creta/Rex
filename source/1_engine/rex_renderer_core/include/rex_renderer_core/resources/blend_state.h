#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

namespace rex
{
  namespace rhi
  {
    // Specifies blend factors, which modulate values for the pixel shader and render target.
    enum class Blend
    {
      Zero,           // The blend factor is (0, 0, 0, 0). No pre-blend operation.
      One,            // The blend factor is (1, 1, 1, 1). No pre-blend operation.
      SrcColor,       // The blend factor is (R, G, B, A), that is color data (RGB) from a pixel shader. No pre-blend operation.
      InvSrcColor,    // The blend factor is (1 - R, 1 - G, 1 - B, 1 - A), that is color data (RGB) from a pixel shader. The pre-blend operation inverts the data, generating 1 - RGB.
      SrcAlpha,       // The blend factor is (A, A, A, A), that is alpha data (A) from a pixel shader. No pre-blend operation.
      InvSrcAlpha,    // The blend factor is ( 1 - A, 1 - A, 1 - A, 1 - A), that is alpha data (A) from a pixel shader. The pre-blend operation inverts the data, generating 1 - A.
      DestAlpha,      // The blend factor is (A, A, A, A,), that is alpha data from a render target. No pre-blend operation.
      InvDestAlpha,   // The blend factor is (1 - A, 1 - A, 1 - A, 1 - A), that is alpha data from a render target. The pre-blend operation inverts the data, generating 1 - A.
      DestColor,      // The blend factor is (R, G, B, A), that is color data from a render target. No pre-blend operation.
      InvDestColor,   // The blend factor is (1 - R, 1 - G, 1 - B, 1 - A), that is color data from a render target. The pre-blend operation inverts the data, generating 1 - RGB.
      SrcAlphaSat,    // The blend factor is (f, f, f, 1); where f = min(A, 1-A). The pre - blend operation clamps the data to 1 or less.
      BlendFactor,    // The blend factor is the blend factor set with ID3D12GraphicsCommandList::OMSetBlendFactor. No pre-blend operation.
      InvBlendFactor, // The blend factor is the blend factor set with ID3D12GraphicsCommandList::OMSetBlendFactor. The pre-blend operation inverts the blend factor, generating 1 - blend_factor.
      Src1Color,      // The blend factor is data sources both as color data output by a pixel shader. There is no pre-blend operation. This blend factor supports dual-source color blending.
      InvSrc1Color,   // The blend factor is data sources both as color data output by a pixel shader. The pre-blend operation inverts the data, generating 1 - RGB. This blend factor supports dual-source color blending.
      Src1Alpha,      // The blend factor is data sources as alpha data output by a pixel shader. There is no pre-blend operation. This blend factor supports dual-source color blending.
      InvSrc1Alpha    // The blend factor is data sources as alpha data output by a pixel shader. The pre-blend operation inverts the data, generating 1 - A. This blend factor supports dual-source color blending.
    };

    // Specifies the RGB or alpha blending operation
    enum class BlendOp
    {
      Add,         // Add source 1 and source 2
      Subtract,    // Subtract source 1 from source 2
      RevSubtract, // Subtract source 2 from source 1
      Min,         // Find minimum between source 1 and source 2
      Max          // Find maximum between source 1 and source 2
    };

    // Specifies the logical operations to configure for a render target
    enum class LogicOp
    {
      Clear, // Clears the render target
      Set,   // Sets the render target
      Copy,  // Copies the render target
      CopyInverted, // Performs an inverted copy of the render target
      Noop, // No operation is performed on the render target
      Invert, // Inverts the render target
      And, // Performs a logical and operation on the render target
      Nand, // Performs a logical nand operation on the render target
      Or, // performs a logical or operation on the render target
      Nor, // performs a logical nor operation on the render target
      Xor, // performs a logical xor operatoin on the render target
      Equiv, // performs a logical equal operation on the render target
      AndReverse, // performs a logical and and reverse operation on the render target
      AndInverted, // performs a logical and and invert operation on the render target
      OrReverse, // performs a logical or and reverse operation on the render target
      OrInverted // performs a logical or and invert operation on the render target
    };

    // Write mask to indicate which component of the render target can be written to
    enum class RenderTargetWriteMask
    {
      Red = BIT(0), // Allow data to be stored in the red component
      Green = BIT(1), // Allow data to be stored in the green component
      Blue = BIT(2), // Allow data to be stored in the blue component
      Alpha = BIT(3), // Allow data to be stored in the alpha component
      All = Red | Green | Blue | Alpha // Allow data to be stored in all components
    };

    // Described the blend state for a render target
    struct RenderTargetBlendDesc
    {
      Blend src_blend; // Specifies the operation to perform on the rgb value that the pixel shader outputs
      Blend dst_blend; // Specifies the operation to perform on the current RGB value in the render target
      BlendOp blend_op; // Specifies how to combine the src blend and dst blend values
      Blend src_blend_alpha; // specifies the operation to perform on the alpha value that the pixel shader outputs
      Blend dst_blend_alpha; // specifies the operation to perform on the current alpha value in the render target
      BlendOp blend_op_alpha; // Specifies how to combine the src blend alpha and dst blend alpha values
      LogicOp logic_op; // specifies the logical operation to configure for the render target
      RenderTargetWriteMask render_target_write_mask; // Specifies which components of the render target we'll be writing to
      bool blend_enable; // Specifies whether to enable or disable blending
      bool logic_op_enable; // Specifies whether to enable or disable a logical operation
    };

    // Describes the blend state
    struct BlendDesc
    {
      bool enable_alpha_to_coverage; // Specifies whether to use alpha-to-coverage as a multisampling technique when setting a pixel to a render target (https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-blend-state#alpha-to-coverage)
      bool independent_blend_state; // Specifies whether to enable independent blending in simultaneous render targets. If false, only render_target[0] members are used, render_target[1,7] are ignored
      RenderTargetBlendDesc render_target[8]; // an array that specifies the blend states for each render target. if "independent_blend_state" is true, only the first is used and shared between all render targets, the rest is ignored.
    };
  }
}