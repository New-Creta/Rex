#include "rex_engine/serialization/text_loaders/tileset_loader_json.h"

#include "rex_engine/assets/tileset_asset.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/filesystem/filesystem.h"
#include "rex_engine/text_processing/json.h"
#include "rex_engine/serialization/image_loading.h"

#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/graphics.h"

#include "rex_std/bonus/math.h"

namespace rex
{
	TilesetLoaderJson::TilesetLoaderJson()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".json" }
			})
	{}

	rsl::unique_ptr<Asset> TilesetLoaderJson::load(rsl::string_view assetPath, LoadFlags loadFlags)
	{
		REX_ASSERT_X(loadFlags == LoadFlags::None, "Tilesets cannot be partially loaded, therefore load flags should always be none");

		rex::json::json json_content = rex::json::read_from_file(assetPath);

		rsl::pointi8 tile_size{};
		tile_size.x = json_content["tile_size"]["x"];
		tile_size.y = json_content["tile_size"]["y"];

		rsl::string_view tileset = json_content["tileset"];
		rex::memory::Blob content = rex::vfs::instance()->read_file(tileset);
		ImageLoadResult tileset_img_load_res = load_image(content);

		// A tileset only holds 1 channel, we have to convert it to 4 channels as that's what the GPU expects
		rsl::unique_array<rsl::Rgba> tileset_rgba = rsl::make_unique<rsl::Rgba[]>(tileset_img_load_res.width * tileset_img_load_res.height * sizeof(rsl::Rgba));
		for (s32 color_idx = 0; color_idx < tileset_img_load_res.width * tileset_img_load_res.height; ++color_idx)
		{
			u8 color = tileset_img_load_res.data[color_idx];
			rsl::Rgba& rgba = tileset_rgba[color_idx];
			rgba.red = color;
			rgba.green = color;
			rgba.blue = color;
			rgba.alpha = 255;
		}

		rsl::unique_ptr<rex::gfx::Texture2D> texture = rex::gfx::gal::instance()->create_texture2d(tileset_img_load_res.width, tileset_img_load_res.height, rex::gfx::TextureFormat::Unorm4, tileset_rgba.get());
		return rsl::make_unique<TilesetAsset>(tile_size, rsl::move(texture));
	}
	void TilesetLoaderJson::hydrate_asset(Asset* asset, rsl::string_view assetPath)
	{
		REX_UNUSED_PARAM(asset);
		REX_UNUSED_PARAM(assetPath);

		REX_ASSERT("Cannot hydra tiletsets as they cannot be partially loaded");
		// Nothing to implement
	}

}