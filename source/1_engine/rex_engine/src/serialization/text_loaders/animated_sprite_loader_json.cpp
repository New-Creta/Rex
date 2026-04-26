#include "rex_engine/serialization/text_loaders/animated_sprite_loader_json.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/assets/flipbook.h"
#include "rex_engine/assets/animated_sprite.h"

#include "rex_engine/text_processing/json.h"

namespace rex
{
	AnimatedSpriteLoaderJson::AnimatedSpriteLoaderJson()
		: AssetLoader(
			{
				rsl::version(0, 0, 1),
				{ ".json" }
			}
		)
	{
		
	}

	rsl::unique_ptr<Asset> AnimatedSpriteLoaderJson::load(rsl::string_view assetPath, LoadFlags /*loadFlags*/)
	{
		rex::json::json json_blob = rex::json::read_from_file(assetPath);
		rsl::string_view flipbook_path = json_blob["flipbook"];

		Flipbook* flipbook = rex::asset_db::instance()->load<Flipbook>(flipbook_path);
		return rsl::make_unique<AnimatedSprite>(flipbook);
	}
	void AnimatedSpriteLoaderJson::hydrate_asset(Asset* /*asset*/, rsl::string_view /*assetPath*/)
	{
		// Nothing to implement
	}
}