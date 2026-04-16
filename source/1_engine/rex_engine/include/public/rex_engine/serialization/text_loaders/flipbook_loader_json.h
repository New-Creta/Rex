#pragma once

#include "rex_engine/serialization/asset_loader.h"

namespace rex
{
	class FlipbookLoaderJson : public AssetLoader
	{
	public:
		FlipbookLoaderJson();

		rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags);

	private:

	};
}