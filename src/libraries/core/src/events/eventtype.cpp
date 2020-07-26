#include "rex_core_pch.h"
#include "events/eventtype.h"

//-------------------------------------------------------------------------
rex::events::EventType::EventType(Type value)
    :m_value(value)
{}

//-------------------------------------------------------------------------
rex::events::EventType::operator Type() const
{
    return m_value;
}

//-------------------------------------------------------------------------
uint32 rex::events::EventType::toUint() const
{
    return (uint32)m_value;
}

//-------------------------------------------------------------------------
std::string rex::events::EventType::toString() const
{
    switch (m_value)
    {
    case rex::events::EventType::KEY_PRESSED:               return "Key Pressed";
    case rex::events::EventType::KEY_RELEASED:              return "Key Released";
    case rex::events::EventType::MOUSE_BUTTON_PRESSED:      return "Mouse Button Pressed";
    case rex::events::EventType::MOUSE_BUTTON_RELEASED:     return "Mouse Button Released";
    case rex::events::EventType::MOUSE_MOVED:               return "Mouse Moved";
    case rex::events::EventType::MOUSE_SCROLLED:            return "Mouse Scrolled";
    case rex::events::EventType::WINDOW_CLOSED:             return "Window Closed";
    case rex::events::EventType::WINDOW_RESIZE:             return "Window Resized";
    }

    //RX_ASSERT_X(false ,"Invalid event category!");
    return "";
}
