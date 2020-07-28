#include "events/input/win32keystate.h"

//-------------------------------------------------------------------------
rex::win32::KeyState::KeyState(LPARAM lparam)
	: data(lparam)
{}