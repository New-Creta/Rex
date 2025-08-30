#pragma once

#include "rex_std/vector.h"
#include "rex_std/string_view.h"
#include "rex_std/functional.h"

#include "imgui/imgui.h"

#include "regina/widgets/widget.h"

struct ImGuiWindow;

namespace regina
{
	class ContentBrowserHiearchyItem
	{
		using on_selected_delegate = rsl::function<void(rsl::string_view)>;

	public:
		ContentBrowserHiearchyItem(rsl::string_view path);

		// Returns the path that's open or an empty string if it's not open
		void update();
		void draw(rsl::string_view selectedDirectory);

		void on_selected(const on_selected_delegate& callback);

	private:
		void draw_background_as_selected(ImGuiWindow* window);

	private:
		rsl::string m_fullpath;
		rsl::string_view m_dir_name;
		rsl::vector<rsl::unique_ptr<ContentBrowserHiearchyItem>> m_sub_items;
		ImGuiID m_id;
		bool m_is_open;
		bool m_selected_last_frame;

		on_selected_delegate m_on_selected_callback;
	};
}