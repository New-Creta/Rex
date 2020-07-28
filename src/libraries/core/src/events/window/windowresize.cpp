#include "events/window/windowresize.h"

//-------------------------------------------------------------------------
rex::events::WindowResize::WindowResize(rex::Window* window, uint32 width, uint32 height)
    : WindowEvent(window)
    , m_width(width), m_height(height)
{
    RX_TRACE("Window resize event created");
}

//-------------------------------------------------------------------------
uint32 rex::events::WindowResize::getWidth() const 
{
    return m_width; 
}
//-------------------------------------------------------------------------
uint32 rex::events::WindowResize::getHeight() const 
{
    return m_height; 
}

//-------------------------------------------------------------------------
std::string rex::events::WindowResize::toString() const
{
    std::stringstream s;
    s << "Window Resized: [" << getWidth() << "," << getHeight() << "]";
    return s.str();
}
