#include "rex_core_pch.h"
#include "events/windowresizedevent.h"

//-------------------------------------------------------------------------
rex::events::WindowResizedEvent::WindowResizedEvent(int width, int height)
    :WindowEvent()
    ,m_width(width)
    ,m_height(height)
{}
//-------------------------------------------------------------------------
rex::events::WindowResizedEvent::~WindowResizedEvent() = default;

//-------------------------------------------------------------------------
int rex::events::WindowResizedEvent::getWidth() const
{
    return m_width;
}
//-------------------------------------------------------------------------
int rex::events::WindowResizedEvent::getHeight() const
{
    return m_height;
}

//-------------------------------------------------------------------------
std::string rex::events::WindowResizedEvent::toString() const
{
    std::stringstream ss;
    ss << "WindowResizeEvent: " << m_width << ", " << m_height;
    return ss.str();
}

