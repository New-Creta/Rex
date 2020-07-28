#pragma once

#include "rex_platform_global.h"

#include "events/window/windowresize.h"

namespace rex
{
	namespace win32
	{
		class WindowResize : public events::WindowResize
		{
		public:
			REX_PLATFORM_EXPORT WindowResize(rex::Window* window, LRESULT param);
		};
	}
}