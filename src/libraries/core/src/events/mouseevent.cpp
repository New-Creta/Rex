#include "rex_core_pch.h"
#include "events/mouseevent.h"

//-------------------------------------------------------------------------
rex::events::MouseEvent::MouseEvent(int x, int y)
    :m_x(x)
    ,m_y(y)
{}
//-------------------------------------------------------------------------
rex::events::MouseEvent::~MouseEvent() = default;

//-------------------------------------------------------------------------
int rex::events::MouseEvent::getX() const
{
    return m_x;
}
//-------------------------------------------------------------------------
int rex::events::MouseEvent::getY() const
{
    return m_y;
}

