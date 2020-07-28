#include "events/input/keyevent.h"

//-------------------------------------------------------------------------
rex::events::KeyEvent::KeyEvent(rex::Window* window, const int8* key)
	: InputEvent(window)
	, m_key(key)
{}

//-------------------------------------------------------------------------
rex::events::KeyEvent::~KeyEvent() = default;

//-------------------------------------------------------------------------
const int8* rex::events::KeyEvent::getKey() const
{
	return m_key;
}
