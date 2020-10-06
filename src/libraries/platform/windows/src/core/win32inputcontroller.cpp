#include "core/win32inputcontroller.h"
#include "core/win32application.h"
#include "core/win32window.h"

namespace conversion
{
	//-------------------------------------------------------------------------
	POINT to_win32_point(const rex::Point64& p)
	{
		POINT point;

		point.x = static_cast<LONG>(p.x);
		point.y = static_cast<LONG>(p.y);

		return point;
	}
	//-------------------------------------------------------------------------
	rex::Point64 to_rex_point(const POINT& p)
	{
		rex::Point64 point;

		point.x = static_cast<int64>(p.x);
		point.y = static_cast<int64>(p.y);

		return point;
	}
}

//-------------------------------------------------------------------------
rex::win32::InputController::InputController()
{
	m_keyboard_state_0 = new BYTE[256];
	m_keyboard_state_1 = new BYTE[256];

	GetKeyboardState(m_keyboard_state_0);
	GetKeyboardState(m_keyboard_state_1);

	m_main_window = static_cast<win32::Application*>(CoreApplication::getInstance())->getWindow();
}
//-------------------------------------------------------------------------
rex::win32::InputController::~InputController()
{
	delete[] m_keyboard_state_0;
	delete[] m_keyboard_state_1;

}

//-------------------------------------------------------------------------
void rex::win32::InputController::update()
{
	BOOL getKeyboardResult;
	if (m_keyboard_state_0_active)
	{
		getKeyboardResult = GetKeyboardState(m_keyboard_state_1);
		m_previous_keyboard_state = m_keyboard_state_0;
		m_current_keyboard_state = m_keyboard_state_1;
	}
	else
	{
		getKeyboardResult = GetKeyboardState(m_keyboard_state_0);
		m_previous_keyboard_state = m_keyboard_state_1;
		m_current_keyboard_state = m_keyboard_state_0;
	}

	m_keyboard_state_0_active = !m_keyboard_state_0_active;

	POINT old_mouse_position = conversion::to_win32_point(m_current_mouse_position);
	POINT current_mouse_position = conversion::to_win32_point(m_current_mouse_position);

	if (GetCursorPos(&current_mouse_position))
	{
		ScreenToClient((HWND)m_main_window->getNativeWindow(), &current_mouse_position);
	}

	m_old_mouse_position = conversion::to_rex_point(old_mouse_position);
	m_current_mouse_position = conversion::to_rex_point(current_mouse_position);

	m_mouse_movement.x = m_current_mouse_position.x - m_old_mouse_position.x;
	m_mouse_movement.y = m_current_mouse_position.y - m_old_mouse_position.y;
}

//-------------------------------------------------------------------------
rex::Point64 rex::win32::InputController::getMousePosition(bool previousFrame) const
{
	return (previousFrame) ? m_old_mouse_position : m_current_mouse_position;
}
//-------------------------------------------------------------------------
rex::Point64 rex::win32::InputController::getMouseMovement() const
{
	return m_mouse_movement;
}

//-------------------------------------------------------------------------
void rex::win32::InputController::showMouseCursor(bool show)
{
	ShowCursor(show);
}

//-------------------------------------------------------------------------
bool rex::win32::InputController::IsMouseButtonDown(rex::MouseCode button) const
{
	switch (button)
	{
	case rex::MouseCode::LEFT: return ((m_previous_keyboard_state[VK_LBUTTON] & 0xF0) != 0 && (m_current_keyboard_state[VK_LBUTTON] & 0xF0) != 0);
	case rex::MouseCode::RIGHT: return ((m_previous_keyboard_state[VK_RBUTTON] & 0xF0) != 0 && (m_current_keyboard_state[VK_RBUTTON] & 0xF0) != 0);
	case rex::MouseCode::MIDDLE: return ((m_previous_keyboard_state[VK_MBUTTON] & 0xF0) != 0 && (m_current_keyboard_state[VK_MBUTTON] & 0xF0) != 0);
	}

	return false;
}
//-------------------------------------------------------------------------
bool rex::win32::InputController::IsMouseButtonUp(rex::MouseCode button) const
{
	switch (button)
	{
	case rex::MouseCode::LEFT: return ((m_previous_keyboard_state[VK_LBUTTON] & 0xF0) == 0 && (m_current_keyboard_state[VK_LBUTTON] & 0xF0) == 0);
	case rex::MouseCode::RIGHT: return ((m_previous_keyboard_state[VK_RBUTTON] & 0xF0) == 0 && (m_current_keyboard_state[VK_RBUTTON] & 0xF0) == 0);
	case rex::MouseCode::MIDDLE: return ((m_previous_keyboard_state[VK_MBUTTON] & 0xF0) == 0 && (m_current_keyboard_state[VK_MBUTTON] & 0xF0) == 0);
	}

	return false;
}
//-------------------------------------------------------------------------
bool rex::win32::InputController::IsMouseButtonPressed(rex::MouseCode button) const
{
	switch (button)
	{
	case rex::MouseCode::LEFT : return ((m_previous_keyboard_state[VK_LBUTTON] & 0xF0) == 0 && (m_current_keyboard_state[VK_LBUTTON] & 0xF0) != 0);
	case rex::MouseCode::RIGHT: return ((m_previous_keyboard_state[VK_RBUTTON] & 0xF0) == 0 && (m_current_keyboard_state[VK_RBUTTON] & 0xF0) != 0);
	case rex::MouseCode::MIDDLE: return ((m_previous_keyboard_state[VK_MBUTTON] & 0xF0) == 0 && (m_current_keyboard_state[VK_MBUTTON] & 0xF0) != 0);
	}

	return false;
}
//-------------------------------------------------------------------------
bool rex::win32::InputController::IsMouseButtonReleased(rex::MouseCode button) const
{
	switch (button)
	{
	case rex::MouseCode::LEFT: return ((m_previous_keyboard_state[VK_LBUTTON] & 0xF0) != 0 && (m_current_keyboard_state[VK_LBUTTON] & 0xF0) == 0);
	case rex::MouseCode::RIGHT: return ((m_previous_keyboard_state[VK_RBUTTON] & 0xF0) != 0 && (m_current_keyboard_state[VK_RBUTTON] & 0xF0) == 0);
	case rex::MouseCode::MIDDLE: return ((m_previous_keyboard_state[VK_MBUTTON] & 0xF0) != 0 && (m_current_keyboard_state[VK_MBUTTON] & 0xF0) == 0);
	}

	return false;
}

//-------------------------------------------------------------------------
bool rex::win32::InputController::isKeyDown(rex::KeyCode code) const
{
	int key = static_cast<int>(code);

	return ((m_previous_keyboard_state[key] & 0xF0) != 0 && (m_current_keyboard_state[key] & 0xF0) != 0);
}
//-------------------------------------------------------------------------
bool rex::win32::InputController::isKeyUp(rex::KeyCode code) const
{
	int key = static_cast<int>(code);

	return ((m_previous_keyboard_state[key] & 0xF0) == 0 && (m_current_keyboard_state[key] & 0xF0) == 0);
}
//-------------------------------------------------------------------------
bool rex::win32::InputController::isKeyPressed(rex::KeyCode code) const
{
	int key = static_cast<int>(code);

	return ((m_previous_keyboard_state[key] & 0xF0) == 0 && (m_current_keyboard_state[key] & 0xF0) != 0);
}
//-------------------------------------------------------------------------
bool rex::win32::InputController::isKeyReleased(rex::KeyCode code) const
{
	int key = static_cast<int>(code);

	return ((m_previous_keyboard_state[key] & 0xF0) != 0 && (m_current_keyboard_state[key] & 0xF0) == 0);
}
