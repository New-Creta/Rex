#include "regina/thumbnail_manager.h"

#include "rex_engine/assets/texture_asset.h"
#include "rex_engine/engine/asset_db.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/gfx/graphics.h"

#include "rex_std/bonus/math/color.h"

namespace regina
{
	ThumbnailManager::ThumbnailManager()
	{
		m_directory_thumbnail = rsl::make_unique<Thumbnail>(rex::asset_db::instance()->load<rex::TextureAsset>(rex::path::join("regina", "icons", "folder.png")));
		m_file_thumbnail = rsl::make_unique<Thumbnail>(rex::asset_db::instance()->load<rex::TextureAsset>(rex::path::join("regina", "icons", "file.png")));
		m_unknown_thumbnail = rsl::make_unique<Thumbnail>(rex::asset_db::instance()->load<rex::TextureAsset>(rex::path::join("regina", "icons", "unknown.png")));
	}

	bool ThumbnailManager::has_thumbnail(rsl::string_view /*path*/) const
	{
		return false;
	}
	const Thumbnail* ThumbnailManager::thumbnail_for_path(rsl::string_view /*path*/) const
	{
		return nullptr;
	}

	const Thumbnail* ThumbnailManager::directory_thumbnail() const
	{
		return m_directory_thumbnail.get();
	}
	const Thumbnail* ThumbnailManager::file_thumbnail() const
	{
		return m_file_thumbnail.get();
	}
	const Thumbnail* ThumbnailManager::unknown_thumbnail() const
	{
		return m_unknown_thumbnail.get();
	}
}