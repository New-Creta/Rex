#include "events/input/win32keyup.h"

#include "core/window.h"

//-------------------------------------------------------------------------
rex::win32::KeyUp::KeyUp(rex::Window* window, rex::win32::MessageParameters params)
	: rex::events::KeyUp(window, (const char*)&params.wparam)
	, rex::win32::NativeEvent(params)
	, m_key_state(params.lparam)
{}

//-------------------------------------------------------------------------
rex::win32::KeyUp::~KeyUp() = default;
