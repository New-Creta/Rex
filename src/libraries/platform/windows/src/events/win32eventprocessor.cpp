#include "events/win32eventprocessor.h"

// input events
#include "events/input/win32keydown.h"
#include "events/input/win32keyup.h"
#include "events/input/win32keytyped.h"
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

//-------------------------------------------------------------------------
rex::win32::EventProcessor::EventProcessor(rex::Window* window, EventCallbackFn callback)
    :m_window(window)
    ,m_callback(callback)
	,m_enabled(false)
{}
//-------------------------------------------------------------------------
rex::win32::EventProcessor::~EventProcessor() = default;

//-------------------------------------------------------------------------
void rex::win32::EventProcessor::enableEventProcessing()
{
	m_enabled = true;
}
//-------------------------------------------------------------------------
void rex::win32::EventProcessor::disableEventProcessing()
{
	m_enabled = false;
}

#pragma warning( push )
#pragma warning( disable : 4239 )

//-------------------------------------------------------------------------
LRESULT rex::win32::EventProcessor::process(HWND hwnd, rex::win32::MessageParameters parameters)
{
	if (!m_enabled)
		return 0;

	//
	// The event message from windows
	//
	UINT msg = parameters.msg;

	switch (msg)
	{
#ifdef _OPENGL
    case WM_ERASEBKGND:
        return 0;
#endif

	case WM_SYSCOMMAND:
	{
		// Disable ALT application menu
        if ((parameters.wparam & 0xfff0) == SC_KEYMENU)																return 0;

		switch (parameters.wparam)
		{
		case SC_SCREENSAVE:																							return 0;
		case SC_MONITORPOWER:																						return 0;
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
	case WM_KEYDOWN:		m_callback(KeyDown(m_window, parameters));												return 0;

	case WM_CHAR:           m_callback(KeyTyped(m_window, parameters));												return 0;
	case WM_KEYUP:          m_callback(KeyUp(m_window, parameters));												return 0;

		//
		// Window events
		//
	case WM_SIZE:			        
		if (parameters.wparam != SIZE_MINIMIZED)
		{
			m_callback(WindowResize(m_window, parameters));
			return 0;
		}

	case WM_ACTIVATE:
		if (parameters.wparam == WA_ACTIVE || parameters.wparam == WA_CLICKACTIVE)
		{
			m_callback(events::WindowActivated(m_window));															return 0;
		}
		else if (parameters.wparam == WA_INACTIVE)
		{
			m_callback(events::WindowDeactivated(m_window));														return 0;
		}

	case WM_SHOWWINDOW:		m_callback(events::WindowOpen(m_window));												return 0;
	case WM_CLOSE:			m_callback(events::WindowClose(m_window));												return 0;

	case WM_DESTROY:		PostQuitMessage(0);																		return 0;
	}

	return DefWindowProc(hwnd, msg, parameters.wparam, parameters.lparam);
}

#pragma warning( pop )