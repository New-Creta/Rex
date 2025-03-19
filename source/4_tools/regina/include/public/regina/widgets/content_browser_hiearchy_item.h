#pragma once

#include "rex_std/vector.h"
#include "rex_std/string_view.h"
#include "imgui/imgui.h"

struct ImGuiWindow;

namespace regina
{
	class ContentBrowserHiearchyItem
	{
	public:
		ContentBrowserHiearchyItem(rsl::string_view path);

		// Returns the path that's open or an empty string if it's not open
		rsl::string_view draw(rsl::string_view selectedDirectory);

	private:
		void draw_background_as_selected(ImGuiWindow* window);

	private:
		rsl::string m_fullpath;
		rsl::string_view m_dir_name;
		rsl::vector<rsl::unique_ptr<ContentBrowserHiearchyItem>> m_sub_items;
		ImGuiID m_id;
		bool m_is_open;
	};
}