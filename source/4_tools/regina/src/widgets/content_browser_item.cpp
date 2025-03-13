#include "regina/widgets/content_browser_item.h"

#include "rex_engine/filesystem/path.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"

namespace regina
{
	ContentBrowserItem::ContentBrowserItem(rsl::string_view name, const Thumbnail* thumbnail)
		: m_path(name)
	{
		m_name = rex::path::filename(m_path);
		m_id = ImGui::GetID(m_name.data());
		m_infopanel_name = rsl::string("InfoPanel") + m_name;
		m_thumbnail = thumbnail;
	}

	void ContentBrowserItem::draw()
	{
		//REX_PROFILE_SCOPE(rsl::format("Rendering {}", item));

		//ImGui::PushID(m_id);
		//ImGui::BeginGroup();

		//f32 edgeOffset = 6.0f;

		//f32 textLineHeight = ImGui::GetTextLineHeightWithSpacing() + edgeOffset * 2.0f;
		//f32 thumbnailSize = 128.0f;
		//f32 thumbBhWidth = 102.0f;
		//f32 thumbBhHeight = 102.0f;
		//f32 infoPanelWidth = thumbnailSize;
		//f32 infoPanelHeight = textLineHeight;
		//f32 itemSpacing = 1.0f;
		//rex::imgui::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(itemSpacing, 0.0f));

		//// Draw background
		//const ImVec2 topLeft = ImGui::GetCursorScreenPos();
		//const ImVec2 thumbBottomRight = { topLeft.x + thumbBhWidth, topLeft.y + thumbBhHeight };
		//const ImVec2 infoTopLeft = { topLeft.x,				 thumbBottomRight.y };
		//const ImVec2 bottomRight = { topLeft.x + infoPanelWidth, infoTopLeft.y + infoPanelHeight };
		//auto* drawList = ImGui::GetWindowDrawList();

		{
			//REX_PROFILE_SCOPE(rsl::format("Rendering background {}", item));
			//if (m_content_selection.is_selected(ImGui::GetID(item.data())))
			//{
			//	drawList->AddRectFilled(topLeft, thumbBottomRight, rex::imgui::highlight);
			//}
			//else
			//{
			//	drawList->AddRectFilled(topLeft, thumbBottomRight, rex::imgui::backgroundDark);
			//}
			//drawList->AddRectFilled(infoTopLeft, bottomRight, rex::imgui::groupHeader, 6.0f, ImDrawFlags_RoundCornersBottom);
		}

		// Shift the offset so we leave some space between the thumbnail and the corner
		//rex::imgui::shift_cursor(edgeOffset, edgeOffset);

		// render the thumbnail
		//{
		//	//REX_PROFILE_SCOPE(rsl::format("Rendering thumbnail {}", item));

		//	//fullpath = rex::path::join(m_current_directory, item);
		//	//const Thumbnail* thumbnail = thumbnail_for_path(fullpath);
		//	ImVec2 imageSize{ 100.0f, 100.0f };
		//	ImGui::Image((ImTextureID)m_thumbnail->texture(), imageSize);
		//}
		// Render Info Panel
		//ImGui::BeginVertical(m_infopanel_name.c_str(), ImVec2(thumbnailSize - edgeOffset * 2.0f, infoPanelHeight - edgeOffset));
		//{
		//	//REX_PROFILE_SCOPE(rsl::format("Rendering infopanel {}", item));

		//	// Centre align directory name
		//	ImGui::BeginHorizontal(m_name.data(), ImVec2(thumbnailSize - 2.0f, 0.0f));
		//	ImGui::Spring();
		//	{
		//		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (thumbBhWidth - edgeOffset));
		//		const f32 textWidth = std::min(ImGui::CalcTextSize(m_name.data()).x, thumbnailSize);
		//		//if (m_IsRenaming)
		//		//{
		//		//	ImGui::SetNextItemWidth(thumbnailSize - edgeOffset * 3.0f);
		//		//	renamingWidget();
		//		//}
		//		//else
		//		//{
		//		ImGui::SetNextItemWidth(textWidth);
		//		ImGui::Text(m_name.data());
		//		//}
		//		ImGui::PopTextWrapPos();
		//	}
		//	ImGui::Spring();
		//	ImGui::EndHorizontal();

		//	ImGui::Spring();
		//}
		//ImGui::EndVertical();

		// Put the cursor back where it came from
		//rex::imgui::shift_cursor(-edgeOffset, -edgeOffset);
		//ImGui::EndGroup();

		// UI actions
		//if (ImGui::IsItemClicked())
		//{
		//	//REX_PROFILE_SCOPE(rsl::format("UI Actions infopanel {}", item));

		//	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		//	{
		//		if (rex::directory::exists(fullpath))
		//		{
		//			change_directory(fullpath);
		//		}
		//		else
		//		{
		//			rex::open_file(fullpath);
		//		}
		//	}
		//	else if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl))
		//	{
		//		m_content_selection.toggle(ImGui::GetID(item.data()));
		//	}
		//	//else if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftShift))
		//	//{
		//	//	
		//	//}
		//	else
		//	{
		//		m_content_selection.clear();
		//		m_content_selection.add(ImGui::GetID(item.data()));
		//	}
		//}
		//else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		//{
		//	m_content_selection.clear();
		//}

		// Based on the action of the user, update the UI
		// the following actions are supported
		// Selection cleared - done
		// item selected - done
		// item deselected - done
		// ctrl + selection
		// rename
		// open in explorer
		// hovered
		// duplicated
		// renamed
		// refresh

		//ImGui::PopID();
	}
}