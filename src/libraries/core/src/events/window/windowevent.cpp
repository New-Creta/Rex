#include "events/window/windowevent.h"

//-------------------------------------------------------------------------
rex::events::WindowEvent::WindowEvent(rex::Window* window)
	: Event()
	, m_window(window)
{}

//-------------------------------------------------------------------------
rex::events::WindowEvent::~WindowEvent() = default;

//-------------------------------------------------------------------------
std::string rex::events::WindowEvent::toString() const
{
	return getCategory().toString() + " " + getType().toString();
}