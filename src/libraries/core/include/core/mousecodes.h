#pragma once

namespace rex
{
    enum class MouseCode
    {
        Button0 = 0,
        Button1 = 1,
        Button2 = 2,
    };

    inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
    {
        os << static_cast<uint16>(mouseCode);
        return os;
    }
}

#define RX_MOUSE_BUTTON_0 rex::MouseCode::Button0
#define RX_MOUSE_BUTTON_1 rex::MouseCode::Button1
#define RX_MOUSE_BUTTON_2 rex::MouseCode::Button2
