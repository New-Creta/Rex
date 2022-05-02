#include "rex_events_pch.h"

#include "event_category.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        EventCategory::EventCategory(uint16 value)
            : m_value((Type)value)
        {
        }
        //-------------------------------------------------------------------------
        EventCategory::EventCategory(Type value)
            : m_value(value)
        {
        }

        //-------------------------------------------------------------------------
        EventCategory::operator Type() const
        {
            return m_value;
        }

        //-------------------------------------------------------------------------
        uint16 EventCategory::to_uint() const
        {
            return (uint16)m_value;
        }

        //-------------------------------------------------------------------------
        std::string EventCategory::to_string() const
        {
            switch (m_value)
            {
                case EventCategory::NONE: return "None";
                case EventCategory::INPUT: return "Input";
                case EventCategory::KEYBOARD: return "Keyboard";
                case EventCategory::MOUSE: return "Mouse";
                case EventCategory::WINDOW: return "Window";
                default: 
                    // R_ASSERT_X(false, "Unknown event category"); 
                    return "None";
            }
        }
    }
}