#include "rex_core_pch.h"
#include "events/mousescrolledevent.h"

//-------------------------------------------------------------------------
rex::events::MouseScrolledEvent::MouseScrolledEvent(int deltaX, int deltaY, int x, int y)
    :MouseEvent(x, y)
    ,m_delta_x(deltaX)
    ,m_delta_y(deltaY)
{}
//-------------------------------------------------------------------------
rex::events::MouseScrolledEvent::~MouseScrolledEvent() = default;

//-------------------------------------------------------------------------
int rex::events::MouseScrolledEvent::getScollX() const
{
    return m_delta_x;
}
//-------------------------------------------------------------------------
int rex::events::MouseScrolledEvent::getScollY() const
{
    return m_delta_y;
}

//-------------------------------------------------------------------------
std::string rex::events::MouseScrolledEvent::toString() const
{
    std::stringstream ss;
    ss << "MouseScrolledEvent: scroll x: " << getScollX() << ", scroll y: " << getScollY();
    return ss.str();
}

