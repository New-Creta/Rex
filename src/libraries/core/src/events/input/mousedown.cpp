#include "events/input/MouseDown.h"

//-------------------------------------------------------------------------
rex::events::MouseDown::MouseDown(rex::Window* window, const MouseKeyMetaData& keyMetaData, MouseCode button, IsDoubleClick isDoubleClick, const MousePosition& mousePos)
    : MouseEvent(window, keyMetaData, mousePos)
    , m_button(button)
    , m_is_double_click(isDoubleClick)
{
    RX_TRACE("Mouse down event created");
}

//-------------------------------------------------------------------------
rex::events::MouseDown::~MouseDown() = default;

//-------------------------------------------------------------------------
rex::MouseCode rex::events::MouseDown::getButton() const
{
    return m_button;
}

//-------------------------------------------------------------------------
std::string rex::events::MouseDown::toString() const
{
    std::stringstream s;
    s << "Mouse Down: " << m_button;
    return s.str();
}
