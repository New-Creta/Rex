#pragma once

#include "events/input/mousedown.h"

#include "events/win32messageparameters.h"

namespace rex
{
	namespace win32
	{
		class MouseDown : public events::MouseDown
		{
		public:
			MouseDown(rex::Window* window, MouseCode button, IsDoubleClick isDoubleClick, rex::win32::MessageParameters params);
			~MouseDown() final;

		};
	}
}