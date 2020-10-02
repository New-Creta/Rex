#pragma once

#include "events/input/mousedown.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{
	namespace win32
	{
		class MouseDown : public events::MouseDown, public NativeEvent
		{
		public:
			MouseDown(rex::Window* window, MouseCode button, IsDoubleClick isDoubleClick, rex::win32::MessageParameters params);
			~MouseDown() final;

            //-------------------------------------------------------------------------
            inline const NativeEvent* getNativeEvent() const { return this; }
		};
	}
}