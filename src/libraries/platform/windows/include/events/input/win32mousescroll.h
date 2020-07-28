#pragma once

#include "events/input/mousescroll.h"

#include "events/win32messageparameters.h"

namespace rex
{
	namespace win32
	{
		class MouseScroll : public events::MouseScroll
		{
		public:
			MouseScroll(rex::Window* window, rex::win32::MessageParameters parameters);
			~MouseScroll() final;

		};
	}
}