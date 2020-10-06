#pragma once

#include "events/input/MouseUp.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{
	namespace win32
	{
		class MouseUp : public events::MouseUp, public NativeEvent
		{
		public:
			MouseUp(rex::Window* window, MouseCode mouse_button, rex::win32::MessageParameters params);
			~MouseUp() final;

			//-------------------------------------------------------------------------
			inline const NativeEvent* getNativeEvent() const { return this; }
		};
	}
}