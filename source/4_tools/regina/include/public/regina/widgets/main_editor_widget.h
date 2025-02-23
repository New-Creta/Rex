#pragma once

#include "regina//widgets/widget.h"

namespace regina
{
	class MainEditorWidget : public Widget
	{
	public:
		MainEditorWidget();

	protected:
		bool on_update() override;

	private:
		void show_menu_bar();
		void init_widgets();

	private:
		bool m_show_imgui_demo;
	};
}