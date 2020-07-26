#include "rex_core_pch.h"
#include "events/keyevent.h"

//-------------------------------------------------------------------------
rex::events::KeyEvent::KeyEvent(KeyCode code)
    :m_code(code)
{}
//-------------------------------------------------------------------------
rex::events::KeyEvent::~KeyEvent() = default;

//-------------------------------------------------------------------------
rex::KeyCode rex::events::KeyEvent::getKeyCode() const
{
    return m_code;
}
