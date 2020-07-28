#include "events/win32eventprocessor.h"

// input events
#include "events/input/win32keydown.h"
#include "events/input/win32noncharacterdown.h"
#include "events/input/win32keyup.h"
#include "events/input/win32mousemove.h"
#include "events/input/win32mousedown.h"
#include "events/input/win32mouseup.h"
#include "events/input/win32mousescroll.h"

// window events
#include "events/window/windowactivated.h"
#include "events/window/windowdeactivated.h"
#include "events/window/windowopen.h"
#include "events/window/windowclose.h"

#include "events/window/win32windowresize.h"

namespace
{
	//-------------------------------------------------------------------------
	bool is_non_character_key(WPARAM param)
	{
		switch (param)
		{
		case NULL:

			// Control keys
		case VK_CLEAR:
		case VK_MENU:
		case VK_PAUSE:
		case VK_CAPITAL:
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_ESCAPE:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:

			// Arrow keys
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:

			// Extended keys
		case VK_SELECT:
		case VK_PRINT:
		case VK_SNAPSHOT:
		case VK_INSERT:
		case VK_DELETE:
		case VK_HELP:
		case VK_SLEEP:

			// Function keys
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
		case VK_F11:
		case VK_F12:
		case VK_F13:
		case VK_F14:
		case VK_F15:
		case VK_F16:
		case VK_F17:
		case VK_F18:
		case VK_F19:
		case VK_F20:
		case VK_F21:
		case VK_F22:
		case VK_F23:
		case VK_F24:
		case VK_NUMLOCK:
		case VK_SCROLL:
		case VK_LSHIFT:
		case VK_RSHIFT:
		case VK_LCONTROL:
		case VK_RCONTROL:
		case VK_LWIN:
		case VK_RWIN:
		case VK_APPS:
			return true;
		}
		return false;
	}
}

//-------------------------------------------------------------------------
rex::win32::EventProcessor::EventProcessor(rex::Window* window, EventCallbackFn callback)
    :m_window(window)
    ,m_callback(callback)
{}
//-------------------------------------------------------------------------
rex::win32::EventProcessor::~EventProcessor() = default;

//-------------------------------------------------------------------------
LRESULT rex::win32::EventProcessor::process(HWND hwnd, UINT msg, rex::win32::MessageParameters parameters)
{
	switch (msg)
	{
	case WM_SYSCOMMAND:
	{
		switch (parameters.wparam)
		{
		case SC_SCREENSAVE:																																			return 0;
		case SC_MONITORPOWER:																																		return 0;
		}
		break;
	}
	//
	// Input events
	//

	// Mouse events
	case WM_MOUSEWHEEL:     m_callback(MouseScroll(m_window, parameters));											return 0;
	case WM_MOUSEMOVE:      m_callback(MouseMove(m_window, parameters));											return 0;
	case WM_LBUTTONDOWN:    m_callback(MouseDown(m_window, MouseCode::LEFT, IsDoubleClick::NO, parameters));		return 0;
	case WM_LBUTTONUP:      m_callback(MouseUp(m_window, MouseCode::LEFT, parameters));								return 0;
	case WM_LBUTTONDBLCLK:  m_callback(MouseDown(m_window, MouseCode::LEFT, IsDoubleClick::YES, parameters));		return 0;
	case WM_RBUTTONDOWN:    m_callback(MouseDown(m_window, MouseCode::RIGHT, IsDoubleClick::NO, parameters));		return 0;
	case WM_RBUTTONUP:      m_callback(MouseUp(m_window, MouseCode::RIGHT, parameters));							return 0;
	case WM_RBUTTONDBLCLK:  m_callback(MouseDown(m_window, MouseCode::RIGHT, IsDoubleClick::YES, parameters));		return 0;

		// Key events
	case WM_KEYDOWN:
		if (is_non_character_key(parameters.wparam))
		{
							m_callback(NonCharacterKeyDown(m_window, parameters));											return 0;
		}

	case WM_CHAR:           m_callback(KeyDown(m_window, parameters));												return 0;
	case WM_KEYUP:          m_callback(KeyUp(m_window, parameters));												return 0;

		//
		// Window events
		//
	case WM_SIZE:			m_callback(WindowResize(m_window, parameters.lparam));									return 0;

	case WM_ACTIVATE:
		if (parameters.wparam == WA_ACTIVE || parameters.wparam == WA_CLICKACTIVE)
		{
			m_callback(events::WindowActivated(m_window));																	return 0;
		}
		else if (parameters.wparam == WA_INACTIVE)
		{
			m_callback(events::WindowDeactivated(m_window));																return 0;
		}

	case WM_SHOWWINDOW:		m_callback(events::WindowOpen(m_window));														return 0;
	case WM_CLOSE:			m_callback(events::WindowClose(m_window));														return 0;

	case WM_DESTROY:		PostQuitMessage(0);																														return 0;
	}

	return DefWindowProc(hwnd, msg, parameters.wparam, parameters.lparam);
}
