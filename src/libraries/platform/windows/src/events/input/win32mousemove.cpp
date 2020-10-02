#include "events/input/win32MouseMove.h"

#include "core/window.h"

#include "events/win32iohelper.h"

//-------------------------------------------------------------------------
rex::win32::MouseMove::MouseMove(rex::Window* window, rex::win32::MessageParameters params)
	: rex::events::MouseMove(window, rex::win32::getMouseKeyMetaData(params.wparam), rex::win32::getMousePositionFromClient(window, params.lparam))
	, rex::win32::NativeEvent(params)
{}