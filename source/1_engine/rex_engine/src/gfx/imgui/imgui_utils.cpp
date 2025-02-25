#include "rex_engine/gfx/imgui/imgui_utils.h"

namespace rex
{
	namespace imgui
	{
		ScopedWidget::ScopedWidget(const char* name, bool* pOpen, ImGuiWindowFlags flags)
		{
			m_is_open = ImGui::Begin(name, pOpen, flags);
		}
		ScopedWidget::~ScopedWidget()
		{
			ImGui::End();
		}
		ScopedWidget::operator bool() const
		{
			return m_is_open;
		}

		ImColor color_with_multiplied_value(const ImColor& color, float multiplier)
		{
			const ImVec4& colRaw = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRaw.x, colRaw.y, colRaw.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(val * multiplier, 1.0f));
		}

		// src will be added to the dst window
		void dock_to_window(rsl::string_view src, rsl::string_view dst, ImGuiDir dir)
		{
			ImGuiWindow* src_window = ImGui::FindWindowByName(src.data());
			ImGuiWindow* dst_window = ImGui::FindWindowByName(dst.data());

			dock_to_window(src_window, dst_window, dir);
		}

		void dock_to_window(ImGuiWindow* src, ImGuiWindow* dst, ImGuiDir dir)
		{
			ImGuiContext* context = ImGui::GetCurrentContext();

			ImGui::DockContextQueueDock(
				context,
				dst,																		// target
				nullptr,																// target node
				src,																		// payload
				dir,																		// simply add a tab
				0.0f,																		// split ratio
				false																		// split outer
			);
		}

		void shift_cursor_y(float distance)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance);
		}

		bool is_item_hovered(float delayInSeconds, ImGuiHoveredFlags flags)
		{
			return ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delayInSeconds; /*HoveredIdNotActiveTimer*/
		}

		void set_tooltip(std::string_view text, float delayInSeconds, bool allowWhenDisabled, ImVec2 padding)
		{
			if (is_item_hovered(delayInSeconds, allowWhenDisabled ? ImGuiHoveredFlags_AllowWhenDisabled : 0))
			{
				ScopedStyle tooltipPadding(ImGuiStyleVar_WindowPadding, padding);
				ScopedColour textCol(ImGuiCol_Text, textBrighter);
				ImGui::SetTooltip(text.data());
			}
		}
		ImRect rect_offset(const ImRect& rect, float x, float y)
		{
			ImRect result = rect;
			result.Min.x += x;
			result.Min.y += y;
			result.Max.x += x;
			result.Max.y += y;
			return result;
		}
		void shift_cursor(float x, float y)
		{
			const ImVec2 cursor = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y + y));
		}
	}
}