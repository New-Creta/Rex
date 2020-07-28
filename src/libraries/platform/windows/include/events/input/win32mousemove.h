#include "events/input/mousemove.h"

#include "events/win32messageparameters.h"

namespace rex
{ 
	namespace win32
	{
		class MouseMove : public events::MouseMove
		{
		public:
			MouseMove(rex::Window* window, rex::win32::MessageParameters params);
		};
	}
}