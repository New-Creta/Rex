#include "rex_core_pch.h"
#include "events/mousebuttonreleasedevent.h"

//-------------------------------------------------------------------------
rex::events::MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseCode code, int x, int y)
    :MouseButtonEvent(code, x, y)
{}
//-------------------------------------------------------------------------
rex::events::MouseButtonReleasedEvent::~MouseButtonReleasedEvent() = default;

//-------------------------------------------------------------------------
std::string rex::events::MouseButtonReleasedEvent::toString() const
{
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << getMouseCode();
    return ss.str();
}
