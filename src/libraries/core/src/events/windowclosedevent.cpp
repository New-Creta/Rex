#include "rex_core_pch.h"
#include "events/windowclosedevent.h"

//-------------------------------------------------------------------------
rex::events::WindowClosedEvent::WindowClosedEvent() = default;
//-------------------------------------------------------------------------
rex::events::WindowClosedEvent::~WindowClosedEvent() = default;

//-------------------------------------------------------------------------
std::string rex::events::WindowClosedEvent::toString() const
{
    std::stringstream ss;
    ss << "WindowClosedEvent";
    return ss.str();
}
