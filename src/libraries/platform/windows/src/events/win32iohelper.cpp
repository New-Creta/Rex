#include "events/win32iohelper.h"

#include "core/window.h"

#include <Windowsx.h>

namespace
{
	//-------------------------------------------------------------------------
	rex::Point getPosition(LPARAM param)
	{
		return { (uint32)GET_X_LPARAM(param), (uint32)GET_Y_LPARAM(param) };
	}
	//-------------------------------------------------------------------------
	int32 getWheelDelta(WPARAM param)
	{
		return GET_WHEEL_DELTA_WPARAM(param) / WHEEL_DELTA;
	}

	//-------------------------------------------------------------------------
	rex::ScrollDirection getScrollDirection(WPARAM param)
	{
		return getWheelDelta(param) > 1 ? rex::ScrollDirection::UP : rex::ScrollDirection::DOWN;
	}
}

//-------------------------------------------------------------------------
rex::MousePosition rex::win32::getMousePositionFromClient(Window* window, LPARAM param)
{
	MousePosition mouse_pos;

	mouse_pos.local_position = getPosition(param);

	HWND hwnd = static_cast<HWND>(window->getHandle());

	RECT rect;
	GetWindowRect(hwnd, &rect);

	mouse_pos.screen_position.x = rect.left;
	mouse_pos.screen_position.y = rect.top;

	return mouse_pos;
}

//-------------------------------------------------------------------------
rex::MousePosition rex::win32::getMousePositionFromScreen(Window* window, LPARAM param)
{
	MousePosition mouse_pos;

	mouse_pos.screen_position = getPosition(param);
	mouse_pos.local_position = getPosition(param);

	HWND hwnd = static_cast<HWND>(window->getHandle());

	RECT rect;
	GetWindowRect(hwnd, &rect);

	mouse_pos.local_position.x = rect.left - mouse_pos.screen_position.x;
	mouse_pos.local_position.y = rect.top - mouse_pos.screen_position.y;

	return mouse_pos;
}

//-------------------------------------------------------------------------
rex::ScrollMetaData rex::win32::getScrollMetaData(WPARAM param)
{
	ScrollMetaData meta_data;

	meta_data.direction = getScrollDirection(param);
	meta_data.wheel_delta = getWheelDelta(param);

	return meta_data;
}

//-------------------------------------------------------------------------
rex::events::MouseKeyMetaData rex::win32::getMouseKeyMetaData(WPARAM param)
{
	rex::events::MouseKeyMetaData meta_data;

	meta_data.is_ctrl_down = param & MK_CONTROL;
	meta_data.is_shift_down = param & MK_SHIFT;

	return meta_data;
}

