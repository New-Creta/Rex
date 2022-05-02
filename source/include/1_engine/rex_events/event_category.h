#pragma once

namespace rex
{
    namespace events
    {
        class EventCategory
        {
        public:
            enum Type
            {
                NONE            = 0,
                INPUT           = BIT(0),
                KEYBOARD        = BIT(1),
                MOUSE           = BIT(2),
                WINDOW          = BIT(3),
                APPLICATION     = BIT(4)
            };

            EventCategory(uint16 value);
            EventCategory(Type value);

            operator Type() const;

            uint16 to_uint() const;
            std::string to_string() const;

        private:
            Type m_value;
        };
    }
}