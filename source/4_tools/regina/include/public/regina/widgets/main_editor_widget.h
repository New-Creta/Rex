#pragma once

#include "regina//widgets/widget.h"

#include "rex_std/vector.h"

struct ImGuiWindow;

namespace regina
{
	class MainEditorWidget : public Widget
	{
	public:
		MainEditorWidget();

	protected:
		bool on_update() override;

	private:
		void render_menu_bar();
		void render_docking_backpanel();
		void render_viewports();

	private:
		bool m_show_imgui_demo;
		bool m_show_imgui_style_editor;

		ImGuiWindow* m_docking_backpanel;
		rsl::vector<rsl::unique_ptr<Widget>> m_widgets;
	};
}