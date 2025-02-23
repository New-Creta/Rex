#pragma once

#include "imgui/imgui_internal.h"

#include "rex_std/string_view.h"

namespace rex
{
	namespace imgui
	{
		class ScopedStyle
		{
		public:
			ScopedStyle(const ScopedStyle&) = delete;
			ScopedStyle& operator=(const ScopedStyle&) = delete;
			template<typename T>
			ScopedStyle(ImGuiStyleVar styleVar, T value) { ImGui::PushStyleVar(styleVar, value); }
			~ScopedStyle() { ImGui::PopStyleVar(); }
		};

		class ScopedColourStack
		{
		public:
			ScopedColourStack(const ScopedColourStack&) = delete;
			ScopedColourStack& operator=(const ScopedColourStack&) = delete;

			template <typename ColourType, typename... OtherColours>
			ScopedColourStack(ImGuiCol firstColourID, ColourType firstColour, OtherColours&& ... otherColourPairs)
				: m_Count((sizeof... (otherColourPairs) / 2) + 1)
			{
				static_assert ((sizeof... (otherColourPairs) & 1u) == 0,
					"ScopedColourStack constructor expects a list of pairs of colour IDs and colours as its arguments");

				PushColour(firstColourID, firstColour, std::forward<OtherColours>(otherColourPairs)...);
			}

			~ScopedColourStack() { ImGui::PopStyleColor(m_Count); }

		private:
			int m_Count;

			template <typename ColourType, typename... OtherColours>
			void PushColour(ImGuiCol colourID, ColourType colour, OtherColours&& ... otherColourPairs)
			{
				if constexpr (sizeof... (otherColourPairs) == 0)
				{
					ImGui::PushStyleColor(colourID, ImColor(colour).Value);
				}
				else
				{
					ImGui::PushStyleColor(colourID, ImColor(colour).Value);
					PushColour(std::forward<OtherColours>(otherColourPairs)...);
				}
			}
		};

		class ScopedWidget
		{
		public:
			ScopedWidget(const char* name, bool* pOpen = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
			ScopedWidget(const ScopedWidget&) = delete;
			ScopedWidget(ScopedWidget&&) = delete;
			~ScopedWidget();

			ScopedWidget& operator=(const ScopedWidget&) = delete;
			ScopedWidget& operator=(ScopedWidget&&) = delete;

			explicit operator bool() const;

		private:
			bool m_is_open;
		};

		ImColor color_with_multiplied_value(const ImColor& color, float multiplier);
		void dock_to_window(rsl::string_view src, rsl::string_view dst, ImGuiDir dir);
		void dock_to_window(ImGuiWindow* src, ImGuiWindow* dst, ImGuiDir dir);
		void shift_cursor_y(float distance);
	}
}