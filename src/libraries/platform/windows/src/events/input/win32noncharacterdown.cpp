#include "events/input/win32noncharacterdown.h"

#include "core/keynames.h"
#include "core/window.h"

//-------------------------------------------------------------------------
const char* get_key_from_non_character(WPARAM windowsKey)
{
    switch (windowsKey)
    {
    case NULL:				return "";

    case VK_CLEAR:			return rex::keynames::CLEAR;
    case VK_MENU:			return rex::keynames::MENU;
    case VK_PAUSE:			return rex::keynames::PAUSE;
    case VK_CAPITAL:		return rex::keynames::CAPITAL;
    case VK_SHIFT:			return rex::keynames::SHIFT;
    case VK_CONTROL:		return rex::keynames::CTRL;
    case VK_ESCAPE:			return rex::keynames::ESCAPE;
    case VK_PRIOR:			return rex::keynames::PAGE_UP;
    case VK_NEXT:			return rex::keynames::PAGE_DOWN;
    case VK_END:			return rex::keynames::END;
    case VK_HOME:			return rex::keynames::HOME;
    case VK_LEFT:			return rex::keynames::LEFT_ARROW;
    case VK_UP:				return rex::keynames::UP_ARROW;
    case VK_RIGHT:			return rex::keynames::RIGHT_ARROW;
    case VK_DOWN:			return rex::keynames::DOWN_ARROW;
    case VK_SELECT:			return rex::keynames::SELECT;
    case VK_PRINT:			return rex::keynames::PRINT;
    case VK_SNAPSHOT:		return rex::keynames::SNAPSHOT;
    case VK_INSERT:			return rex::keynames::INSERT;
    case VK_DELETE:			return rex::keynames::DELETE;
    case VK_HELP:			return rex::keynames::HELP;
    case VK_SLEEP:			return rex::keynames::SLEEP;
    case VK_F1:				return rex::keynames::F1;
    case VK_F2:				return rex::keynames::F2;
    case VK_F3:				return rex::keynames::F3;
    case VK_F4:				return rex::keynames::F4;
    case VK_F5:				return rex::keynames::F5;
    case VK_F6:				return rex::keynames::F6;
    case VK_F7:				return rex::keynames::F7;
    case VK_F8:				return rex::keynames::F8;
    case VK_F9:				return rex::keynames::F9;
    case VK_F10:			return rex::keynames::F10;
    case VK_F11:			return rex::keynames::F11;
    case VK_F12:			return rex::keynames::F12;
    case VK_F13:			return rex::keynames::F13;
    case VK_F14:			return rex::keynames::F14;
    case VK_F15:			return rex::keynames::F15;
    case VK_F16:			return rex::keynames::F16;
    case VK_F17:			return rex::keynames::F17;
    case VK_F18:			return rex::keynames::F18;
    case VK_F19:			return rex::keynames::F19;
    case VK_F20:			return rex::keynames::F20;
    case VK_F21:			return rex::keynames::F21;
    case VK_F22:			return rex::keynames::F22;
    case VK_F23:			return rex::keynames::F23;
    case VK_F24:			return rex::keynames::F24;
    case VK_NUMLOCK:		return rex::keynames::NUMLOCK;
    case VK_SCROLL:			return rex::keynames::SCROLL;
    case VK_LSHIFT:			return rex::keynames::LEFT_SHIFT;
    case VK_RSHIFT:			return rex::keynames::RIGHT_SHIFT;
    case VK_LCONTROL:		return rex::keynames::LEFT_CTRL;
    case VK_RCONTROL:		return rex::keynames::RIGHT_CTRL;
    case VK_LWIN:           return rex::keynames::LEFT_PLATFORM_BTN;
    case VK_RWIN:           return rex::keynames::RIGHT_PLATFORM_BTN;
    case VK_APPS:           return rex::keynames::APPS;
    }

    RX_ERROR("Key not found!");
    return "";
}


//-------------------------------------------------------------------------
rex::win32::NonCharacterKeyDown::NonCharacterKeyDown(rex::Window* window, rex::win32::MessageParameters params)
    : rex::events::KeyDown(window, get_key_from_non_character(params.wparam))
    , m_key_state(params.lparam)
{}

//-------------------------------------------------------------------------
rex::win32::NonCharacterKeyDown::~NonCharacterKeyDown() = default;
