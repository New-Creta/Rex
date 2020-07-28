#include "events/input/KeyUp.h"

//-------------------------------------------------------------------------
rex::events::KeyUp::KeyUp(rex::Window* window, const int8* key)
    : KeyEvent(window, key)
    , m_key_code(keyNameToKeyCode(key))
{
    RX_TRACE("Key up event created");
}
//-------------------------------------------------------------------------
rex::events::KeyUp::~KeyUp() = default;

//-------------------------------------------------------------------------
rex::KeyCode rex::events::KeyUp::getKeyCode() const
{
    return m_key_code;
}

//-------------------------------------------------------------------------
std::string rex::events::KeyUp::toString() const
{
    std::stringstream s;
    s << "Key Up: key: " << getKey() << ", keycode: " << getKeyCode();
    return s.str();
}
