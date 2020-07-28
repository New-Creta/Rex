#include "events/input/keydown.h"

//-------------------------------------------------------------------------
rex::events::KeyDown::KeyDown(rex::Window* window, const int8* key)
    : KeyEvent(window, key)
    , m_key_code(keyNameToKeyCode(key))
{
    RX_TRACE("Key down event created");
}
//-------------------------------------------------------------------------
rex::events::KeyDown::~KeyDown() = default;

//-------------------------------------------------------------------------
rex::KeyCode rex::events::KeyDown::getKeyCode() const
{
    return m_key_code;
}