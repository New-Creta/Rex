#include "rex_engine/serialization/text_loaders/flipbook_loader_json.h"

#include "rex_engine/text_processing/json.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/assets/texture_asset.h"
#include "rex_engine/assets/flipbook.h"

namespace rex
{
	FlipbookLoaderJson::FlipbookLoaderJson()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".json" }
			})
	{}

	rsl::unique_ptr<Asset> FlipbookLoaderJson::load(rsl::string_view assetPath, LoadFlags loadFlags)
	{
		rex::json::json json_blob = rex::json::read_from_file(assetPath);

		rsl::pointi8 sprite_size;
		sprite_size.x = json_blob["sprite_size"]["x"];
		sprite_size.y = json_blob["sprite_size"]["y"];

		TextureAsset* texture = asset_db::instance()->load<TextureAsset>(json_blob["texture"]);

		rsl::vector<FlipbookAnimation> animations;

		return rsl::make_unique<Flipbook>(sprite_size, texture, rsl::move(animations));
	}
}