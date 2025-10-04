#include "rex_engine/engine/asset_db.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/loading/begin_asset_load.h"
#include "rex_engine/event_system/events/loading/end_asset_load.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogAssetDatabase);

	rsl::string_view AssetDb::asset_path(const Asset* asset)
	{
		if (m_asset_to_metadata.contains(asset))
		{
			return m_asset_to_metadata.at(asset).path;
		}

		return "";
	}

	Asset* AssetDb::load(rsl::type_id_t assetTypeId, rsl::string_view assetPath, LoadFlags flags)
	{
		// If we already have the asset loaded, let's return it 
		Asset* cached_asset = lookup_cached_asset(assetPath, flags);
		if (cached_asset)
		{
			return cached_asset;
		}

		// If the file doesn't exist, we can't load it
		if (!rex::vfs::instance()->exists(assetPath))
		{
			REX_ERROR(LogAssetDatabase, "asset at path {} does not exist", quoted(assetPath));
			return nullptr;
		}

		REX_VERBOSE(LogAssetDatabase, "Loading {}", assetPath);

		// If we don't have an importer for the type, we can't initialize it, so return
		rsl::string_view type_name = assetTypeId.name();
		if (!m_importers.contains(type_name))
		{
			REX_ERROR(LogAssetDatabase, "Cannot load asset '{}' as it does not have an importer", assetPath);
			return nullptr;
		}

		// If none of the loaders of the importer support the asset, we can't initialize it either, so have to return here as well
		rsl::unique_ptr<AssetImporter>& importer = m_importers.at(type_name);
		if (!importer->can_load(assetPath))
		{
			REX_ERROR(LogAssetDatabase, "Cannot load asset '{}' as it does not have required extension.", assetPath);
			REX_ERROR(LogAssetDatabase, "Required extension for asset should be one of the following:");
			rsl::vector<rsl::string_view> allowed_extensions = importer->allowed_extensions();
			for (rsl::string_view ext : allowed_extensions)
			{
				REX_ERROR(LogAssetDatabase, " -{}", ext);
			}

			return nullptr;
		}

		// We know we can load the asset, so fire the event that it's beginning to load
		event_system::instance()->fire_event(BeginAssetLoad(assetPath));

		// Hydrate the asset if it was partially loaded before
		Asset* potentially_partially_loaded_asset = lookup_cached_asset(assetPath, LoadFlags::PartialLoad);
		if (potentially_partially_loaded_asset)
		{
			importer->hydrate_asset(potentially_partially_loaded_asset, assetPath);
			m_asset_to_metadata.at(potentially_partially_loaded_asset).is_partially_loaded = false;
			return potentially_partially_loaded_asset;
		}

		// Deserialize, initialize and cache the asset
		rsl::unique_ptr<Asset> asset(importer->load(assetPath, flags));

		AssetMetaData metadata{};
		metadata.path = rsl::string(assetPath);
		metadata.is_partially_loaded = rsl::has_flag(flags, LoadFlags::PartialLoad);
		m_asset_to_metadata.emplace(asset.get(), metadata);
		auto emplace_result = m_path_to_asset.emplace(assetPath, rsl::move(asset));

		// Asset is loaded, so fire the event that is has fully loaded
		event_system::instance()->fire_event(EndAssetLoad(assetPath, asset.get()));
		return emplace_result.inserted_element->value.get();
	}

	Asset* AssetDb::lookup_cached_asset(rsl::string_view assetPath, LoadFlags loadFlags)
	{
		if (!m_path_to_asset.contains(assetPath))
		{
			return nullptr;
		}

		Asset* asset = m_path_to_asset.at(assetPath).get();
		const AssetMetaData& asset_metadata = m_asset_to_metadata.at(asset);
		if (asset_metadata.is_partially_loaded && !rsl::has_flag(loadFlags, LoadFlags::PartialLoad))
		{
			return nullptr;
		}

		return asset;
	}

	void AssetDb::hydrate_asset(rsl::type_id_t assetTypeId, Asset* asset)
	{
		if (!m_asset_to_metadata.contains(asset))
		{
			return;
		}

		// If it's already hydrated
		if (m_asset_to_metadata.at(asset).is_partially_loaded == false)
		{
			return;
		}

		// If we don't have an importer for the type, we can't initialize it, so return
		rsl::string_view type_name = assetTypeId.name();
		if (!m_importers.contains(type_name))
		{
			REX_ERROR(LogAssetDatabase, "Cannot hydrate asset as it does not have an importer");
			return;
		}

		rsl::string_view asset_path = m_asset_to_metadata.at(asset).path;
		m_importers.at(assetTypeId.name())->hydrate_asset(asset, asset_path);
		m_asset_to_metadata.at(asset).is_partially_loaded = false;
	}

	bool AssetDb::is_partially_loaded(rsl::string_view assetPath)
	{
		if (!m_path_to_asset.contains(assetPath))
		{
			return nullptr;
		}

		Asset* asset = m_path_to_asset.at(assetPath).get();
		return is_partially_loaded(asset);
	}

	bool AssetDb::is_partially_loaded(const Asset* asset)
	{
		if (!asset)
		{
			return false;
		}

		const AssetMetaData& asset_metadata = m_asset_to_metadata.at(asset);
		return asset_metadata.is_partially_loaded;
	}

	namespace asset_db
	{
		globals::GlobalUniquePtr<AssetDb> g_asset_db;
		void init(globals::GlobalUniquePtr<AssetDb> assetDb)
		{
			g_asset_db = rsl::move(assetDb);
		}
		AssetDb* instance()
		{
			return g_asset_db.get();
		}
		void shutdown()
		{
			g_asset_db.reset();
		}
	}
}