#pragma once

#include "regina/widgets/widget.h"
#include "regina/widgets/content_browser_hiearchy_item.h"
#include "regina/widgets/content_browser_item.h"
#include "regina/thumbnail_manager.h"

#include "rex_engine/engine/types.h"

#include "rex_std/string.h"
#include "rex_std/vector.h"

#include "imgui/imgui.h"

namespace regina
{
	class Selection
	{
	public:
		void add(ImGuiID id);
		void remove(ImGuiID id);
		void toggle(ImGuiID id);
		void clear();

		bool is_selected(ImGuiID id) const;

	private:
		void add_without_search(ImGuiID id);
		void remove_it(rsl::vector<ImGuiID>::const_iterator it);

	private:
		rsl::vector<ImGuiID> m_selected_items;
	};

	class ContentBrowserWidget : public Widget
	{
	public:
		ContentBrowserWidget();

	protected:
		bool on_update() override;

	private:
		void render_content_structure_context_menu();

		void render_top_bar(f32 height);
		void render_bottom_bar(f32 height);
		void render_items();

		const Thumbnail* thumbnail_for_path(rsl::string_view path) const;

		void change_directory(rsl::string_view newDirectory);

	private:
		rsl::string m_current_directory;
		rsl::vector<rsl::string> m_files_in_current_directory;
		rsl::vector<rsl::string> m_directories_in_current_directory;
		//rsl::vector<rsl::string_view> m_current_items;
		rsl::vector<rsl::string> m_root_directories;
		rsl::unique_ptr<ThumbnailManager> m_thumbnail_manager;

		rsl::vector<ContentBrowserHiearchyItem> m_hiearchy_items;
		rsl::vector<ContentBrowserItem> m_current_items;

		Selection m_content_selection;
		Selection m_dir_selection;
	};
}