#include "events/input/mouseevent.h"

//-------------------------------------------------------------------------
rex::events::MouseEvent::MouseEvent(rex::Window* window, const MouseKeyMetaData& keyMetaData, const MousePosition& mousPos)
    : InputEvent(window)
    , m_mouse_position(mousPos)
    , m_is_ctrl_down(keyMetaData.is_ctrl_down)
    , m_is_shift_down(keyMetaData.is_shift_down)
{}

//-------------------------------------------------------------------------
rex::events::MouseEvent::~MouseEvent() = default;

//-------------------------------------------------------------------------
const rex::Point& rex::events::MouseEvent::getScreenPosition() const 
{ 
    return m_mouse_position.screen_position;
}

//-------------------------------------------------------------------------
const rex::Point& rex::events::MouseEvent::getLocalPosition() const 
{ 
    return m_mouse_position.local_position;
}

//-------------------------------------------------------------------------
bool rex::events::MouseEvent::isCtrlDown() const
{
    return m_is_ctrl_down;
}
//-------------------------------------------------------------------------
bool rex::events::MouseEvent::isShiftDown() const
{
    return m_is_shift_down;
}