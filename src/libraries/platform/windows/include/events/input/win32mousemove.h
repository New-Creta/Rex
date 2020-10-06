#include "events/input/mousemove.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{ 
	namespace win32
	{
		class MouseMove : public events::MouseMove, public NativeEvent
		{
		public:
			MouseMove(rex::Window* window, rex::win32::MessageParameters params);

            //-------------------------------------------------------------------------
            inline const NativeEvent* getNativeEvent() const { return this; }
		};
	}
}