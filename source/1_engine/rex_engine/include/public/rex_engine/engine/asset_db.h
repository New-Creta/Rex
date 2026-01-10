#pragma once

#include "rex_engine/engine/globals.h"

#include "rex_engine/assets/asset.h"
#include "rex_engine/serialization/asset_importer.h"
#include "rex_engine/serialization/asset_loader.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"

#include "rex_std/bonus/algorithms.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

namespace rex
{
	// Asset dependency tracking should not be done in asset DB
	struct AssetMetaData
	{
		rsl::string path;
		bool is_partially_loaded;
	};

	// The asset database keeps track of asset metadata (eg. where they were loaded from)
	// It also owns the loading of each asset using loaders (similar system as assimp https://github.com/assimp/assimp)
	// each loader can support a set of extensions and is responsible for loading an asset
	// of a matching extension
	// asset loading is type based, meaning, you always assign a loader to a type of asset
	// on load request, the asset db checks if such a loader exists (wrapped in an importer)
	// if it does and the extension matches, it will call the loader to load the asset
	// having a system like this allows for easy extension in both engine and user code
	class AssetDb
	{
	public:
		// Load an asset from disk
		// Always convert the asset path to its full path represenation
		// so that there's no confusion about the path the asset is loaded from
		template <typename T>
		T* load(rsl::string_view assetPath, LoadFlags loadFlags = LoadFlags::None)
		{
			scratch_string fullpath = rex::vfs::instance()->abs_path(assetPath);
			fullpath.replace("\\", "/");
			rsl::to_lower(fullpath.cbegin(), fullpath.begin(), fullpath.length());
			return static_cast<T*>(load(rsl::type_id<T>(), fullpath, loadFlags));
		}

		// Hydrate a previously loaded asset, filling it with data
		template <typename T>
		void hydrate_asset(T* asset)
		{
			hydrate_asset(rsl::type_id<T>(), asset);
		}

		// add/register a new loader, creating a new importer if one doesn't already exist
		template <typename T>
		void add_loader(rsl::unique_ptr<AssetLoader> loader)
		{
			rsl::string_view type_name = rsl::type_id<T>().name();
			if (!m_importers.contains(type_name))
			{
				m_importers.emplace(type_name, rsl::make_unique<AssetImporter>());
			}

			m_importers.at(type_name)->add_loader(rsl::move(loader));
		}

		// return the path where an asset was loaded from, if such a path exists
		// It's possible asset got created at runtime, meaning they won't have a path
		rsl::string_view asset_path(const Asset* asset);

	private:
		Asset* load(rsl::type_id_t assetTypeId, rsl::string_view assetPath, LoadFlags flags);
		Asset* lookup_cached_asset(rsl::string_view assetPath, LoadFlags loadFlags);

		void hydrate_asset(rsl::type_id_t assetTypeId, Asset* asset);
		bool is_partially_loaded(rsl::string_view assetPath);
		bool is_partially_loaded(const Asset* asset);

	private:
		rsl::unordered_map<rsl::string_view, rsl::unique_ptr<AssetImporter>> m_importers;
		rsl::unordered_map<rsl::string, rsl::unique_ptr<Asset>> m_path_to_asset;
		rsl::unordered_map<const Asset*, AssetMetaData> m_asset_to_metadata;
	};

	namespace asset_db
	{
		void init(globals::GlobalUniquePtr<AssetDb> assetDb);
		AssetDb* instance();
		void shutdown();
	}
}