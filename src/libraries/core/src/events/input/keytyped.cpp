#include "events/input/keytyped.h"

//-------------------------------------------------------------------------
rex::events::KeyTyped::KeyTyped(rex::Window* window, const int8* key)
    : KeyEvent(window, key)
{
    RX_TRACE("Key typed event created");
}
//-------------------------------------------------------------------------
rex::events::KeyTyped::~KeyTyped() = default;

//-------------------------------------------------------------------------
std::string rex::events::KeyTyped::toString() const
{
    std::stringstream s;
    s << "Key Down: key: " << *getKey();
    return s.str();
}
