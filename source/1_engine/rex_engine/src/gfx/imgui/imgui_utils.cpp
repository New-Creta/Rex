#include "rex_engine/gfx/imgui/imgui_utils.h"

namespace rex
{
	namespace gfx
	{
		// src will be added to the dst window
		void dock_to_window(rsl::string_view src, rsl::string_view dst, ImGuiDir dir)
		{
			ImGuiContext* context = ImGui::GetCurrentContext();

			ImGui::DockContextQueueDock(
				context,
				ImGui::FindWindowByName(dst.data()),		// target
				nullptr,																// target node
				ImGui::FindWindowByName(src.data()),		// payload
				dir,																		// simply add a tab
				0.0f,																		// split ratio
				false																		// split outer
			);
		}
	}
}