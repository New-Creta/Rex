#include "events/input/KeyUp.h"

//-------------------------------------------------------------------------
rex::events::KeyUp::KeyUp(rex::Window* window, const int8* key)
    : KeyEvent(window, key)
{
    RX_TRACE("Key up event created");
}
//-------------------------------------------------------------------------
rex::events::KeyUp::~KeyUp() = default;

//-------------------------------------------------------------------------
std::string rex::events::KeyUp::toString() const
{
    std::stringstream s;
    s << "Key Up: key: " << *getKey();
    return s.str();
}
