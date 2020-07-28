#include "events/input/MouseScroll.h"

//-------------------------------------------------------------------------
rex::events::MouseScroll::MouseScroll(rex::Window* window, const MouseKeyMetaData& keyMetaData, ScrollMetaData scrollMetaData, const MousePosition& mousePos)
    : MouseEvent(window, keyMetaData, mousePos)
    , m_direction(scrollMetaData.direction)
    , m_wheel_delta(scrollMetaData.wheel_delta)
{
    RX_TRACE("Mouse scroll event created");
}
//-------------------------------------------------------------------------
rex::events::MouseScroll::~MouseScroll() = default;

//-------------------------------------------------------------------------
rex::ScrollDirection rex::events::MouseScroll::getScrollDirection() const
{
    return m_direction;
}
//-------------------------------------------------------------------------
int32 rex::events::MouseScroll::getScrollDelta() const
{
    return m_wheel_delta;
}