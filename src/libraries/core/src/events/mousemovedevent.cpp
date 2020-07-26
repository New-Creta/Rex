#include "rex_core_pch.h"
#include "events/mousemovedevent.h"

//-------------------------------------------------------------------------
rex::events::MouseMovedEvent::MouseMovedEvent(int x, int y)
    :MouseEvent(x, y)
{}
//-------------------------------------------------------------------------
rex::events::MouseMovedEvent::~MouseMovedEvent() = default;

//-------------------------------------------------------------------------
std::string rex::events::MouseMovedEvent::toString() const
{
    std::stringstream ss;
    ss << "MouseMovedEvent: " << getX() << ", " << getY();
    return ss.str();
}
