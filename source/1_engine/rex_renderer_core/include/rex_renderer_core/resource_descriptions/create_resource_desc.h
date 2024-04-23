#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/format.h"

namespace rex
{
  namespace renderer
  {
    enum class ResourceDimension
    {
      DIMENSION_UNKNOWN   = 0,
      DIMENSION_BUFFER    = 1,
      DIMENSION_TEXTURE1D = 2,
      DIMENSION_TEXTURE2D = 3,
      DIMENSION_TEXTURE3D = 4
    };

    enum class TextureLayout
    {
      TEXTURE_LAYOUT_UNKNOWN                = 0,
      TEXTURE_LAYOUT_ROW_MAJOR              = 1,
      TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE = 2,
      TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE  = 3
    };

    enum ResourceFlags
    {
      RESOURCE_FLAG_NONE                        = 0,
      RESOURCE_FLAG_ALLOW_RENDER_TARGET         = 0x1,
      RESOURCE_FLAG_ALLOW_DEPTH_STENCIL         = 0x2,
      RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS      = 0x4,
      RESOURCE_FLAG_DENY_SHADER_RESOURCE        = 0x8,
      RESOURCE_FLAG_ALLOW_CROSS_ADAPTER         = 0x10,
      RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS   = 0x20,
      RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY = 0x40
    };

    struct CreateResourceDesc
    {
      ResourceDimension dimension;
      u64               alignment;
      u64               width;
      u32               height;
      u16               depth_or_array_size;
      u16               mip_levels;
      Format            format;
      u32               sample_desc_count;
      u32               sample_desc_quality;
      TextureLayout     layout;
      ResourceFlags     flags;
    };
  } // namespace renderer
} // namespace rex