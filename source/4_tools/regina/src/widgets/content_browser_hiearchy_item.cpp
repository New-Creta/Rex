#include "regina/widgets/content_browser_hiearchy_item.h"

#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/path.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace regina
{
	DEFINE_LOG_CATEGORY(LogContentBrowserHiearchyItem);

	ContentBrowserHiearchyItem::ContentBrowserHiearchyItem(rsl::string_view path)
		: m_fullpath(path)
		, m_dir_name(rex::path::filename(m_fullpath))
		, m_is_open(false)
	{
		rsl::vector<rsl::string> sub_dirs = rex::directory::list_dirs(path);
		for (rsl::string_view dir : sub_dirs)
		{
			m_sub_items.emplace_back(rsl::make_unique<ContentBrowserHiearchyItem>(dir));
		}
	}

	rsl::string_view ContentBrowserHiearchyItem::draw(rsl::string_view selectedDirectory)
	{
		// Pushing an ID as it's possible directories exist with the same name
		// We do not want these to clash with each other
		ImGui::PushID(m_fullpath.data());

		// The below is done so that we have some spacing between our directory names
		// This makes is a bit easier on the human eye
		auto* window = ImGui::GetCurrentWindow();
		window->DC.CurrLineSize.y = 20.0f;
		window->DC.CurrLineTextBaseOffset = 3.0f;

		if (rex::path::is_same(m_fullpath, selectedDirectory))
		{
			draw_background_as_selected(window);
		}

		ImGuiDockNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
		const bool open = ImGui::TreeNodeEx(m_dir_name.data(), flags);

		// If we weren't open last frame, but we are now, that means the user has toggled the node
		// This means we have to return the path of the current node
		const bool just_opened = !m_is_open && open;

		// boilerplate processing, to make sure we return our fullpath
		// if we selected this node in this frame
		m_is_open = open;
		rsl::string_view just_opened_path = just_opened
			? m_fullpath
			: rsl::string_view("");

		if (open)
		{
			for (auto& sub_item : m_sub_items)
			{
				// If any of the sub items were opened this frame
				// We need to propagate that back to the top so we can handle it
				rsl::string_view just_opened_sub_path = sub_item->draw(selectedDirectory);
				if (!just_opened_sub_path.empty())
				{
					just_opened_path = just_opened_sub_path;
				}
			}

			ImGui::TreePop();
		}

		ImGui::PopID();

		return just_opened_path;
	}

	void ContentBrowserHiearchyItem::draw_background_as_selected(ImGuiWindow* window)
	{
		const ImRect item_rect =
		{
			window->WorkRect.Min.x, window->DC.CursorPos.y,
			window->WorkRect.Max.x, window->DC.CursorPos.y + window->DC.CurrLineSize.y
		};

		ImGui::GetWindowDrawList()->AddRectFilled(item_rect.Min, item_rect.Max, rex::imgui::selection);
	}
}