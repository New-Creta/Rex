#pragma once

#include "imgui/imgui_internal.h"

#include "rex_std/string_view.h"

namespace rex
{
	namespace imgui
	{
		constexpr auto accent = IM_COL32(236, 158, 36, 255);
		constexpr auto highlight = IM_COL32(39, 185, 242, 255);
		constexpr auto niceBlue = IM_COL32(83, 232, 254, 255);
		constexpr auto compliment = IM_COL32(78, 151, 166, 255);
		constexpr auto background = IM_COL32(36, 36, 36, 255);
		constexpr auto backgroundDark = IM_COL32(26, 26, 26, 255);
		constexpr auto titlebar = IM_COL32(21, 21, 21, 255);
		constexpr auto titlebarOrange = IM_COL32(186, 66, 30, 255);
		constexpr auto titlebarGreen = IM_COL32(18, 88, 30, 255);
		constexpr auto titlebarRed = IM_COL32(185, 30, 30, 255);
		constexpr auto propertyField = IM_COL32(15, 15, 15, 255);
		constexpr auto text = IM_COL32(192, 192, 192, 255);
		constexpr auto textBrighter = IM_COL32(210, 210, 210, 255);
		constexpr auto textDarker = IM_COL32(128, 128, 128, 255);
		constexpr auto textError = IM_COL32(230, 51, 51, 255);
		constexpr auto muted = IM_COL32(77, 77, 77, 255);
		constexpr auto groupHeader = IM_COL32(47, 47, 47, 255);
		constexpr auto selection = IM_COL32(237, 192, 119, 255);
		constexpr auto selectionMuted = IM_COL32(237, 201, 142, 23);
		constexpr auto backgroundPopup = IM_COL32(50, 50, 50, 255);
		constexpr auto validPrefab = IM_COL32(82, 179, 222, 255);
		constexpr auto invalidPrefab = IM_COL32(222, 43, 43, 255);
		constexpr auto missingMesh = IM_COL32(230, 102, 76, 255);
		constexpr auto meshNotSet = IM_COL32(250, 101, 23, 255);

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

		class ScopedColour
		{
		public:
			ScopedColour(const ScopedColour&) = delete;
			ScopedColour& operator=(const ScopedColour&) = delete;
			template<typename T>
			ScopedColour(ImGuiCol colourId, T colour) { ImGui::PushStyleColor(colourId, ImColor(colour).Value); }
			~ScopedColour() { ImGui::PopStyleColor(); }
		};

		class ScopedFont
		{
		public:
			ScopedFont(const ScopedFont&) = delete;
			ScopedFont& operator=(const ScopedFont&) = delete;
			ScopedFont(ImFont* font) { ImGui::PushFont(font); }
			~ScopedFont() { ImGui::PopFont(); }
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
		bool is_item_hovered(float delayInSeconds = 0.1f, ImGuiHoveredFlags flags = 0);
		void set_tooltip(std::string_view text, float delayInSeconds = 0.1f, bool allowWhenDisabled = true, ImVec2 padding = ImVec2(5, 5));
		ImRect rect_offset(const ImRect& rect, float x, float y);
		void shift_cursor(float x, float y);
	}
}