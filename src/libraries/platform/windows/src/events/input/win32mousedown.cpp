#include "events/input/win32MouseDown.h"

#include <Windowsx.h>

#include "core/window.h"

#include "events/win32iohelper.h"

//-------------------------------------------------------------------------
rex::win32::MouseDown::MouseDown(rex::Window* window, MouseCode button, IsDoubleClick isDoubleClick, rex::win32::MessageParameters params)
	: rex::events::MouseDown(window, getMouseKeyMetaData(params.wparam), button, isDoubleClick , getMousePositionFromClient(window, params.lparam))
{}

//-------------------------------------------------------------------------
rex::win32::MouseDown::~MouseDown() = default;
