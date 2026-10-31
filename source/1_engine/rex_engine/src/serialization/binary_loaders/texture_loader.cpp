#include "rex_engine/serialization/binary_loaders/texture_loader.h"

#include "rex_engine/assets/texture_asset.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/serialization/image_loading.h"

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

		gfx::TextureFormat format = image_load_res.num_channels == 1
			? gfx::TextureFormat::Unorm1
			: gfx::TextureFormat::Unorm4;

		return rsl::make_unique<TextureAsset>(image_load_res.data.get(), image_load_res.width, image_load_res.height, format);
	}
}