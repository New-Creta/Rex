#include "pokemon/serialization/binary_loaders/tileset_loader.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/serialization/image_loading.h"

#include "rex_engine/assets/texture_asset.h"

#include "pokemon/old_code/tileset.h"

namespace pokemon
{
	TilesetLoader::TilesetLoader()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".png" }
			})
	{
	}

	rsl::unique_ptr<rex::Asset> TilesetLoader::load(rsl::string_view assetPath, rex::LoadFlags loadFlags)
	{
		rex::memory::Blob content = rex::vfs::instance()->read_file(assetPath);
		rex::ImageLoadResult image_load_res = rex::load_image(content);

		rex::gfx::TextureFormat format = image_load_res.num_channels == 1
			? rex::gfx::TextureFormat::Unorm1
			: rex::gfx::TextureFormat::Unorm4;

		return rsl::make_unique<Tileset>(image_load_res.data.get(), image_load_res.width, image_load_res.height, format);
	}
}