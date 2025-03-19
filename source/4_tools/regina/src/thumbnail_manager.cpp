#include "regina/thumbnail_manager.h"

#include "rex_engine/images/image_loader.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/gfx/system/gal.h"

#include "rex_std/bonus/math/color.h"

namespace regina
{
	ThumbnailManager::ThumbnailManager()
	{
		m_directory_thumbnail = create_thumbnail_from_path(rex::path::join("regina", "icons", "folder.png"));
		m_file_thumbnail = create_thumbnail_from_path(rex::path::join("regina", "icons", "file.png"));
		m_unknown_thumbnail = create_thumbnail_from_path(rex::path::join("regina", "icons", "unknown.png"));
	}

	bool ThumbnailManager::has_thumbnail(rsl::string_view path) const
	{
		return false;
	}
	const Thumbnail* ThumbnailManager::thumbnail_for_path(rsl::string_view path) const
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

	rsl::unique_ptr<Thumbnail> ThumbnailManager::create_thumbnail_from_path(rsl::string_view path)
	{
		rex::ImageLoadResult img = rex::load_image(path);
		rsl::Rgba* img_data = reinterpret_cast<rsl::Rgba*>(img.data.get());
		rsl::unique_ptr<rex::gfx::Texture2D> tileset_texture = rex::gfx::gal()->create_texture2d(img.width, img.height, rex::gfx::TextureFormat::Unorm4, img_data);

		return rsl::make_unique<Thumbnail>(rsl::move(tileset_texture));
	}

}