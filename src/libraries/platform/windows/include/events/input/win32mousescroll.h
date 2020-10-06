#pragma once

#include "events/input/mousescroll.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{
	namespace win32
	{
		class MouseScroll : public events::MouseScroll, public NativeEvent
		{
		public:
			MouseScroll(rex::Window* window, rex::win32::MessageParameters parameters);
			~MouseScroll() final;

            //-------------------------------------------------------------------------
            inline const NativeEvent* getNativeEvent() const { return this; }
		};
	}
}