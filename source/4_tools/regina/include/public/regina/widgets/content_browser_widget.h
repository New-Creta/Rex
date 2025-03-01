#pragma once

#include "regina//widgets/widget.h"
#include "regina/thumbnail_manager.h"

#include "rex_engine/engine/types.h"

#include "rex_std/string.h"
#include "rex_std/vector.h"

namespace regina
{
	class ContentBrowserWidget : public Widget
	{
	public:
		ContentBrowserWidget();

	protected:
		bool on_update() override;

	private:
		void render_directory_hiearchy(rsl::string_view directory);
		void render_content_structure_context_menu();

		void render_top_bar(f32 height);
		void render_bottom_bar(f32 height);
		void render_items();

		const Thumbnail* thumbnail_for_path(rsl::string_view path) const;

	private:
		rsl::string m_current_directory;
		rsl::vector<rsl::string> m_files_in_current_directory;
		rsl::vector<rsl::string> m_directories_in_current_directory;
		rsl::unique_ptr<ThumbnailManager> m_thumbnail_manager;
	};
}