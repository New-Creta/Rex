#include "events/window/win32windowresize.h"

//-------------------------------------------------------------------------
rex::win32::WindowResize::WindowResize(rex::Window * window, rex::win32::MessageParameters params)
	: rex::events::WindowResize(window, LOWORD(params.lparam), HIWORD(params.lparam))
	, rex::win32::NativeEvent(params)
{

}
