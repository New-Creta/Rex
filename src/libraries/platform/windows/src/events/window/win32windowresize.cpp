#include "events/window/win32windowresize.h"

//-------------------------------------------------------------------------
rex::win32::WindowResize::WindowResize(rex::Window* window, LPARAM param)
	: rex::events::WindowResize(window, LOWORD(param), HIWORD(param))
{}