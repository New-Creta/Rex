#include "rex_core_pch.h"
#include "events/mousebuttonpressedevent.h"

//-------------------------------------------------------------------------
rex::events::MouseButtonPressedEvent::MouseButtonPressedEvent(MouseCode code, int x, int y)
    :MouseButtonEvent(code, x, y)
{}
//-------------------------------------------------------------------------
rex::events::MouseButtonPressedEvent::~MouseButtonPressedEvent() = default;

//-------------------------------------------------------------------------
std::string rex::events::MouseButtonPressedEvent::toString() const
{
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << getMouseCode();
    return ss.str();
}
