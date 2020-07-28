#include "events/input/win32MouseUp.h"

#include "core/window.h"

#include "math/point.h"

#include "events/win32iohelper.h"

#include <windowsx.h>

//-------------------------------------------------------------------------
rex::win32::MouseUp::MouseUp(rex::Window* window, MouseCode mouse_button, rex::win32::MessageParameters params)
	: rex::events::MouseUp(window, rex::win32::getMouseKeyMetaData(params.wparam), mouse_button, getMousePositionFromClient(window, params.lparam))
{}

//-------------------------------------------------------------------------
rex::win32::MouseUp::MouseUp::~MouseUp() = default;
