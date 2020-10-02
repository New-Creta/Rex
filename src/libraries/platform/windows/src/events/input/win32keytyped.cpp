#include "events/input/win32keytyped.h"

#include "core/window.h"

//-------------------------------------------------------------------------
rex::win32::KeyTyped::KeyTyped(rex::Window* window, rex::win32::MessageParameters params)
    : rex::events::KeyTyped(window, (const char*)&params.wparam)
    , rex::win32::NativeEvent(params)
    , m_key_state(params.lparam)
{}

//-------------------------------------------------------------------------
rex::win32::KeyTyped::~KeyTyped() = default;
