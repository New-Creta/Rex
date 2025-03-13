#pragma once

#include "regina/thumbnail.h"

#include "imgui/imgui.h"

namespace regina
{
	class ContentBrowserItem
	{
	public:
		ContentBrowserItem(rsl::string_view path, const Thumbnail* thumbnail);

		void draw();

	private:
		rsl::string m_path;
		rsl::string m_infopanel_name;
		rsl::string_view m_name;
		ImGuiID m_id;
		const Thumbnail* m_thumbnail;
	};
}