#include "events/input/win32MouseScroll.h"

#include <windowsx.h>

#include "core/window.h"

#include "events/win32iohelper.h"

//-------------------------------------------------------------------------
rex::win32::MouseScroll::MouseScroll(rex::Window* window, rex::win32::MessageParameters parameters)
	: rex::events::MouseScroll(window, getMouseKeyMetaData(parameters.wparam), getScrollMetaData(parameters.wparam), getMousePositionFromScreen(window, parameters.lparam))
{}

//-------------------------------------------------------------------------
rex::win32::MouseScroll::~MouseScroll() = default;

