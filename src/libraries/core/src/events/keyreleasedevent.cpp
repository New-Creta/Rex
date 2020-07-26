#include "rex_core_pch.h"
#include "events/keyreleasedevent.h"

//-------------------------------------------------------------------------
rex::events::KeyReleasedEvent::KeyReleasedEvent(KeyCode code)
    :KeyEvent(code)
{}
//-------------------------------------------------------------------------
rex::events::KeyReleasedEvent::~KeyReleasedEvent() = default;

//-------------------------------------------------------------------------
std::string rex::events::KeyReleasedEvent::toString() const
{
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << getKeyCode();
    return ss.str();
}
