#pragma once

#include "rex_core_global.h"

#include "helpers/types.h"
#include "helpers/defines.h"

namespace rex
{
    namespace events
    {
        class EventCategory
        {
        public:
            enum Type
            {
                NONE = 0,
                INPUT       = BIT(0),
                KEYBOARD    = BIT(1),
                MOUSE       = BIT(2),
                WINDOW      = BIT(3),
            };

            REX_CORE_EXPORT EventCategory(uint16 value);
            REX_CORE_EXPORT EventCategory(Type value);

            REX_CORE_EXPORT operator Type() const;

            REX_CORE_EXPORT uint16 toUint() const;
            REX_CORE_EXPORT std::string toString() const;

        private:
            Type m_value;
        };
    }
}