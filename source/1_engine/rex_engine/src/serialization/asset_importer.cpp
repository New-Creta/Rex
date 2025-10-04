#include "rex_engine/serialization/asset_importer.h"

#include "rex_std/bonus/utility/output_param.h"

namespace rex
{
	void AssetImporter::add_loader(rsl::unique_ptr<AssetLoader> loader)
	{
		m_loaders.push_back(rsl::move(loader));
	}

	bool AssetImporter::can_load(rsl::string_view assetPath) const
	{
		for (const rsl::unique_ptr<AssetLoader>& loader : m_loaders)
		{
			if (loader->can_load(assetPath))
			{
				return true;
			}
		}

		return false;
	}
	rsl::vector<rsl::string_view> AssetImporter::allowed_extensions() const
	{
		rsl::vector<rsl::string_view> allowed_extensions;

		for (const rsl::unique_ptr<AssetLoader>& loader : m_loaders)
		{
			loader->allowed_extensions(rsl::Out(allowed_extensions));
		}

		return allowed_extensions;
	}
	rsl::unique_ptr<Asset> AssetImporter::load(rsl::string_view assetPath, LoadFlags loadFlags)
	{
		rsl::vector<rsl::string_view> allowed_extensions;

		for (rsl::unique_ptr<AssetLoader>& loader : m_loaders)
		{
			if (loader->can_load(assetPath))
			{
				return loader->load(assetPath, loadFlags);
			}
		}

		return nullptr;
	}
	void AssetImporter::hydrate_asset(Asset* asset, rsl::string_view assetPath)
	{
		rsl::vector<rsl::string_view> allowed_extensions;

		for (rsl::unique_ptr<AssetLoader>& loader : m_loaders)
		{
			if (loader->can_load(assetPath))
			{
				loader->hydrate_asset(asset, assetPath);
				return;
			}
		}
	}
}