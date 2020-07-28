#include "events/input/MouseUp.h"

//-------------------------------------------------------------------------
rex::events::MouseUp::MouseUp(rex::Window* window, const MouseKeyMetaData& keyMetaData, MouseCode button, const MousePosition& mousePos)
    : MouseEvent(window, keyMetaData, mousePos)
    , m_button(button)
{
    RX_TRACE("Mouse up event created");
}

//-------------------------------------------------------------------------
rex::events::MouseUp::~MouseUp() = default;

//-------------------------------------------------------------------------
rex::MouseCode rex::events::MouseUp::getButton() const
{
    return m_button;
}