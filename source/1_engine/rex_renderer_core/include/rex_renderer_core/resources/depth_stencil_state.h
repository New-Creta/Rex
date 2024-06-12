#pragma once

#include "rex_renderer_core/rhi/comparison_func.h"

namespace rex
{
  namespace rhi
  {
    // Mask to enable/disable writes to the depth stencil buffer
    enum class DepthWriteMask
    {
      DepthWriteMaskZero, // Turn off write to the depth stencil buffer
      DepthWriteMaskAll   // Turn on writes to the depth stencil buffer
    };

    // Possible stencil operation to be performed
    enum class StencilOp
    {
      Keep,     // Keep the existing stencil data
      Zero,     // Set the stencil data to 0
      Replace,  // Replace the stencil data to what's set on the context
      IncrSat,  // Increment the stencil value by 1 and clamp the result
      DecrSat,  // Decrement the stencil value by 1 and clamp the result
      Invert,   // Invert the stencil data
      Incr,     // Increment the stencil value by 1, and wrap the result if necessary
      Decr      // Decrement the stencil value by 1, and wrap the result if necessary
    };

    // Describes stencil operations that can be performed based on the results of the stencil test
    struct DepthStencilOpDesc
    {
      StencilOp stencil_fail_op;          // Stencil operation to perform when stencil testing fails
      StencilOp stencil_depth_fail_op;    // Stencil operation to perform when stencil testing passed and depth testing fails
      StencilOp stencil_pass_op;          // Stencil operation to perform when stencil and depth testing passes.
      renderer::ComparisonFunc stencil_func;  // Function describing what to do with the stencil and the existing stencil data
    };

    // Describes the depth stencil state
    struct DepthStencilDesc
    {
      DepthWriteMask depth_write_mask;      // Identify a portion of the depth-stencil buffer that can be modified by depth state
      renderer::ComparisonFunc depth_func;  // Function describing how to compare depth data against existing depth data
      DepthStencilOpDesc front_face;        // Describes how the results of the depth and stencil test for pixels whose surface pixel is facing towards from the camera
      DepthStencilOpDesc back_face;         // Describes how the results of the depth and stencil test for pixels whose surface pixel is facing away from the camera
      u8 stencil_read_mask;                 // Identify a portion of the depth-stencil buffer for reading stencil data
      u8 stencil_write_mask;                // Identify a portion of the depth-stencil buffer for writing stencil data
      bool depth_enable;                    // Specifies whether to enable depth testing
      bool stencil_enable;                  // Specifies whether to enable stencil testing
    };
  }
}