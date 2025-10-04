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

		return rsl::make_unique<TextureAsset>(rsl::move(image_load_res.data), image_load_res.width, image_load_res.height, image_load_res.num_channels);
	}
}