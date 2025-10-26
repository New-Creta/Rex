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
		rsl::unique_array<rsl::Rgba> rgba = rsl::make_unique<rsl::Rgba[]>(image_load_res.width * image_load_res.height * sizeof(rsl::Rgba));
		for (s32 color_idx = 0; color_idx < image_load_res.width * image_load_res.height; ++color_idx)
		{
			u8 color = image_load_res.data[color_idx];
			rsl::Rgba& rgba_color = rgba[color_idx];
			rgba_color.red = color;
			rgba_color.green = color;
			rgba_color.blue = color;
			rgba_color.alpha = 255;
		}

		return rsl::make_unique<TextureAsset>(rgba.get(), image_load_res.width, image_load_res.height, image_load_res.num_channels);
	}
}