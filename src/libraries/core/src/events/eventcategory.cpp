#include "rex_core_pch.h"
#include "events/eventcategory.h"

//-------------------------------------------------------------------------
rex::events::EventCategory::EventCategory(uint16 value)
    :m_value((Type)value)
{

}
//-------------------------------------------------------------------------
rex::events::EventCategory::EventCategory(Type value)
    :m_value(value)
{

}

//-------------------------------------------------------------------------
rex::events::EventCategory::operator Type() const
{
    return m_value;
}

//-------------------------------------------------------------------------
uint16 rex::events::EventCategory::toUint() const
{
    return (uint16)m_value;
}

//-------------------------------------------------------------------------
std::string rex::events::EventCategory::toString() const
{
    switch (m_value)
    {
    case rex::events::EventCategory::INPUT:     return "Input";
    case rex::events::EventCategory::KEYBOARD:  return "Keyboard";
    case rex::events::EventCategory::MOUSE:     return "Mouse";
    case rex::events::EventCategory::WINDOW:    return "Window";
    }

    //RX_ASSERT_X(false ,"Invalid event category!");
    return "";
}

