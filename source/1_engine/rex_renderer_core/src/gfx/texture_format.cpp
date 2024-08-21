#include "rex_renderer_core/gfx/texture_format.h"

#include "rex_std/bonus/utility.h"

namespace rex
{
	namespace gfx
	{
		s32 format_byte_size(TextureFormat format)
		{
      switch (format)
      {
      case rex::gfx::TextureFormat::Unorm4:            return 4 * sizeof(char8);
      case rex::gfx::TextureFormat::Unorm3:            return 3 * sizeof(char8);
      case rex::gfx::TextureFormat::Unorm4Srgb:        return 4 * sizeof(char8);
      case rex::gfx::TextureFormat::Depth32:           return 4 * sizeof(char8);
      case rex::gfx::TextureFormat::Depth24Stencil8:   return 4 * sizeof(char8);
      default:
        REX_ASSERT("Unknown texture format, cannot determine its size. {}", rsl::enum_refl::enum_name(format));
          break;
      }

      return -1;
		}

    bool is_depth_format(TextureFormat format)
    {
      switch (format)
      {
      case rex::gfx::TextureFormat::Depth32:
      case rex::gfx::TextureFormat::Depth24Stencil8: return true;
      default: return false;
      }
    }

	}
}