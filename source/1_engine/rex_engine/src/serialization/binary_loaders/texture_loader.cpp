#include "rex_engine/serialization/binary_loaders/texture_loader.h"

#include "rex_engine/assets/texture_asset.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/serialization/image_loading.h"

#include "rex_std/bonus/math.h"

namespace rex
{
	TextureLoader::TextureLoader()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".png" }
			})
	{}

	rsl::unique_ptr<Asset> TextureLoader::load(rsl::string_view assetPath, LoadFlags /*loadFlags*/)
	{
		rex::memory::Blob content = rex::vfs::instance()->read_file(assetPath);
		ImageLoadResult image_load_res = load_image(content);

		// A tileset only holds 1 channel, we have to convert it to 4 channels as that's what the GPU expects
		// originally coming from .2bpp assets with the following color pallete
		// 0b00 -> white
		// 0b01 -> light grey
		// 0b10 -> dark grey
		// 0b11 -> black
		REX_STATIC_WARNING("This code always expects 2bpp format and will therefore fail for other textures. That should get fixed");
		rsl::unique_array<rsl::Rgba> rgba = rsl::make_unique<rsl::Rgba[]>(image_load_res.width * image_load_res.height * sizeof(rsl::Rgba));
		for (s32 texel_idx = 0; texel_idx < image_load_res.width * image_load_res.height; ++texel_idx)
		{
			u8 color = image_load_res.data[texel_idx];
			rsl::Rgba& rgba_color = rgba[texel_idx];
			rgba_color.red = color;
			rgba_color.green = color;
			rgba_color.blue = color;
			rgba_color.alpha = 255;
		}

		return rsl::make_unique<TextureAsset>(rgba.get(), image_load_res.width, image_load_res.height, image_load_res.num_channels);
	}
}