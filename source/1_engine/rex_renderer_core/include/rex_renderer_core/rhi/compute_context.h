#pragma once

#include "rex_renderer_core/rhi/graphics_context.h"
#include "rex_renderer_core/gfx/compute_engine.h"

namespace rex
{
  namespace rhi
  {
    class ComputeContext : public GraphicsContext
    {
    public:
      ComputeContext(gfx::GraphicsEngine* owningEngine);

      // Copy Buffer
      // Copy Texture
      // Copy Resource (any that's not a buffer or a texture)
      // Copy Tiles
      // Transition Resource States

      // Clear State
      // Clear Unordered Access View Float
      // Clear Unordered ACcess View Uint
      // Discard Resource
      // Dispatch
      // ExecuteIndirect
      // Set Compute Root Constant
      // Set Compute Root Constants
      // Set Compute Root Constant Buffer View
      // Set Compute Root Descriptor Table
      // Set Compute Root Shader Resource View
      // Set Compute Root Signature
      // Set Compute Root Unordered Access View
      // Set Descriptor Heaps
      // Set Pipeline State
      // Set Predication
      // End Query
    };
  }
}