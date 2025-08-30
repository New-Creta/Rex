#include "regina/widgets/content_browser_hiearchy_item.h"

#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/path.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"
#include "rex_engine/gfx/imgui/imgui_colors.h"

#include "imgui/imgui_internal.h"

namespace regina
{
	DEFINE_LOG_CATEGORY(LogContentBrowserHiearchyItem);

	ContentBrowserHiearchyItem::ContentBrowserHiearchyItem(rsl::string_view path)
		: m_fullpath(path)
		, m_dir_name(rex::path::filename(m_fullpath))
		, m_is_open(false)
		, m_selected_last_frame(false)
	{
		m_id = ImGui::GetID(m_fullpath.c_str());

		rsl::vector<rsl::string> sub_dirs = rex::directory::list_dirs(path);
		for (rsl::string_view dir : sub_dirs)
		{
			rsl::unique_ptr<ContentBrowserHiearchyItem>& hiearchy_item = m_sub_items.emplace_back(rsl::make_unique<ContentBrowserHiearchyItem>(rex::path::join(m_fullpath, dir)));
			hiearchy_item->on_selected([this](rsl::string_view fullpath)
				{
					m_on_selected_callback(fullpath);
				});
		}
	}

	void ContentBrowserHiearchyItem::update()
	{
		if (m_selected_last_frame)
		{
			m_on_selected_callback(m_fullpath);
			m_selected_last_frame = false;
		}

		for (auto& sub_item : m_sub_items)
		{
			sub_item->update();
		}
	}
	void ContentBrowserHiearchyItem::draw(rsl::string_view selectedDirectory)
	{
		// Pushing an ID as it's possible directories exist with the same name
		// We do not want these to clash with each other
		ImGui::PushID(m_id);

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
		m_selected_last_frame = !m_is_open && open;
		m_is_open = open;

		if (m_is_open)
		{
			for (auto& sub_item : m_sub_items)
			{
				sub_item->draw(selectedDirectory);
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void ContentBrowserHiearchyItem::on_selected(const on_selected_delegate& callback)
	{
		m_on_selected_callback = callback;
	}

	void ContentBrowserHiearchyItem::draw_background_as_selected(ImGuiWindow* window)
	{
		const ImRect item_rect =
		{
			window->WorkRect.Min.x, window->DC.CursorPos.y,
			window->WorkRect.Max.x, window->DC.CursorPos.y + window->DC.CurrLineSize.y
		};

		ImGui::GetWindowDrawList()->AddRectFilled(item_rect.Min, item_rect.Max, rex::imgui::colors::selection);
	}
}