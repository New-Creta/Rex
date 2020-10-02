#pragma once

#include "events/nativeevent.h"

namespace rex
{
    namespace events
    {
        class GenericEvent : public NativeEvent
        {
        public:
            GenericEvent() = default;
            ~GenericEvent() override = default;;

            //-------------------------------------------------------------------------
            const void* getData() const { return nullptr; }
        };
    }
}