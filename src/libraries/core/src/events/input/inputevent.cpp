#include "events/input/inputevent.h"

//-------------------------------------------------------------------------
rex::events::InputEvent::InputEvent(rex::Window* window)
    : Event()
    , m_window(window)
{}

//-------------------------------------------------------------------------
rex::events::InputEvent::~InputEvent() = default;

//-------------------------------------------------------------------------
std::string rex::events::InputEvent::toString() const
{
    return getCategory().toString() + " " + getType().toString();
}