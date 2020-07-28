#pragma once

#include "events/input/MouseUp.h"

#include "events/win32messageparameters.h"

namespace rex
{
	namespace win32
	{
		class MouseUp : public events::MouseUp
		{
		public:
			MouseUp(rex::Window* window, MouseCode mouse_button, rex::win32::MessageParameters params);
			~MouseUp() final;

		};
	}
}