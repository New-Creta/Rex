#pragma once

#include "rex_engine/serialization/asset_loader.h"

namespace rex
{
	class TextureLoader : public AssetLoader
	{
	public:
		TextureLoader();

		rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags) override;
	};
}