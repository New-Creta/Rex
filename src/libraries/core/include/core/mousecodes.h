#pragma once

namespace rex
{
    enum class MouseCode
    {
        LEFT = 0,
        MIDDLE = 1,
        RIGHT = 2,
    };

    inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
    {
        os << static_cast<uint16>(mouseCode);
        return os;
    }
}

#define RX_MOUSE_BUTTON_LEFT rex::MouseCode::LEFT
#define RX_MOUSE_BUTTON_MIDDLE rex::MouseCode::MIDDLE
#define RX_MOUSE_BUTTON_RIGHT rex::MouseCode::RIGHT
