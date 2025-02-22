#pragma once

#include "regina//widgets/widget.h"

namespace regina
{
	class MainEditorWidget : public Widget
	{
	public:

	protected:
		bool on_update() override;

	private:
		void show_menu_bar();
	};
}