#include "rex_core_pch.h"
#include "events/mousebuttonevent.h"

//-------------------------------------------------------------------------
rex::events::MouseButtonEvent::MouseButtonEvent(MouseCode code, int x, int y)
    :MouseEvent(x, y)
    ,m_code(code)
{}
//-------------------------------------------------------------------------
rex::events::MouseButtonEvent::~MouseButtonEvent() = default;

//-------------------------------------------------------------------------
rex::MouseCode rex::events::MouseButtonEvent::getMouseCode() const
{
    return m_code;
}