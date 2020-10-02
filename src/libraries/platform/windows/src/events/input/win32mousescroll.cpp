#include "events/input/win32MouseScroll.h"

#include <windowsx.h>

#include "core/window.h"

#include "events/win32iohelper.h"

//-------------------------------------------------------------------------
rex::win32::MouseScroll::MouseScroll(rex::Window* window, rex::win32::MessageParameters params)
	: rex::events::MouseScroll(window, getMouseKeyMetaData(params.wparam), getScrollMetaData(params.wparam), getMousePositionFromScreen(window, params.lparam))
	, rex::win32::NativeEvent(params)
{}

//-------------------------------------------------------------------------
rex::win32::MouseScroll::~MouseScroll() = default;

