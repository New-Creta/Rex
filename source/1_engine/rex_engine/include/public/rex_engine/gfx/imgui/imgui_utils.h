#pragma once

#include "imgui/imgui_internal.h"

#include "rex_std/string_view.h"

namespace rex
{
	namespace gfx
	{
		void dock_to_window(rsl::string_view src, rsl::string_view dst, ImGuiDir dir);
	}
}