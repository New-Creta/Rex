#pragma once

namespace rex
{
    enum class MouseCode
    {
        LEFT    = 1,
        RIGHT   = 2,
        MIDDLE  = 4,
        X1      = 5,
        X2      = 6,
    };

    //-------------------------------------------------------------------------
    inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
    {
        os << static_cast<uint16>(mouseCode);
        return os;
    }
}

#define RX_MOUSE_BUTTON_LEFT    rex::MouseCode::LEFT
#define RX_MOUSE_BUTTON_RIGHT   rex::MouseCode::RIGHT
#define RX_MOUSE_BUTTON_MIDDLE  rex::MouseCode::MIDDLE
#define RX_MOUSE_BUTTON_X1      rex::MouseCode::X1
#define RX_MOUSE_BUTTON_X2      rex::MouseCode::X2
