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
		REX_ASSERT_X(loadFlags == LoadFlags::None, "Flipbooks cannot be partially loaded, therefore load flags should always be none");

		rex::json::json json_blob = rex::json::read_from_file(assetPath);

		rsl::pointi8 sprite_size;
		sprite_size.x = json_blob["sprite_size"]["x"];
		sprite_size.y = json_blob["sprite_size"]["y"];

		TextureAsset* texture = asset_db::instance()->load<TextureAsset>(json_blob["texture"]);

		rsl::vector<FlipbookAnimation> animations;
		rsl::vector<FlipbookAnimationFrame> sprites;
		for (const json::json& anim : json_blob["animations"])
		{
			rsl::string_view name = anim["name"];
			sprites.clear();
			for (const json::json& sprite_json : anim["sprites"])
			{
				FlipbookAnimationFrame sprite{};
				sprite.sprite_idx = sprite_json["idx"];
				sprite.num_frames = sprite_json.value("num_frames", 1);
				sprite.flip_x = sprite_json.value("flip_x", false);
				sprite.flip_y = sprite_json.value("flip_y", false);
				sprites.emplace_back(sprite);
			}
			int ticks_between_interrupt = anim["ticks_between_interrupt"];
			animations.emplace_back(name, rsl::move(sprites), ticks_between_interrupt);
		}

		return rsl::make_unique<Flipbook>(sprite_size, texture, rsl::move(animations));
	}
}