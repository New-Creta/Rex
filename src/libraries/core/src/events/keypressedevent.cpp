#include "rex_core_pch.h"
#include "events/keypressedevent.h"

//-------------------------------------------------------------------------
rex::events::KeyPressedEvent::KeyPressedEvent(KeyCode code, int repeat)
    :KeyEvent(code)
    ,m_repeat_count(repeat)
{}
//-------------------------------------------------------------------------
rex::events::KeyPressedEvent::~KeyPressedEvent() = default;

//-------------------------------------------------------------------------
int rex::events::KeyPressedEvent::getRepeatCount() const
{
    return m_repeat_count;
}

//-------------------------------------------------------------------------
std::string rex::events::KeyPressedEvent::toString() const
{
    std::stringstream ss;
    ss << "KeyPressedEvent: " << getKeyCode() << " (" << getRepeatCount() << " repeats)";
    return ss.str();
}

