#pragma once

#include "rex_platform_global.h"

#include "events/window/windowresize.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{
	namespace win32
	{
		class WindowResize : public events::WindowResize, public NativeEvent
		{
		public:
			REX_PLATFORM_EXPORT WindowResize(rex::Window* window, rex::win32::MessageParameters params);

            //-------------------------------------------------------------------------
            inline const NativeEvent* getNativeEvent() const { return this; }
		};
	}
}