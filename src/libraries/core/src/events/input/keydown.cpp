#include "events/input/keydown.h"

//-------------------------------------------------------------------------
rex::events::KeyDown::KeyDown(rex::Window* window, const int8* key)
    : KeyEvent(window, key)
{
    RX_TRACE("Key down event created");
}
//-------------------------------------------------------------------------
rex::events::KeyDown::~KeyDown() = default;

//-------------------------------------------------------------------------
std::string rex::events::KeyDown::toString() const
{
    std::stringstream s;
    s << "Key Down: key: " << *getKey();
    return s.str();
}
