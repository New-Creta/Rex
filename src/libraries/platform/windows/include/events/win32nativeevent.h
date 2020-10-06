#pragma once

#include "events/nativeevent.h"

#include "events/win32messageparameters.h"

namespace rex
{
    namespace win32
    {
        class NativeEvent : public rex::events::NativeEvent
        {
        public:
            //-------------------------------------------------------------------------
            NativeEvent(const MessageParameters& parameters)
                :rex::events::NativeEvent()
                , m_message_parameters(parameters)
            {}
            //-------------------------------------------------------------------------
            ~NativeEvent() override = default;

            //-------------------------------------------------------------------------
            const void* getData() const override
            {
                return reinterpret_cast<const void*>(&m_message_parameters);
            }
        private:
            MessageParameters m_message_parameters;
        };
    }
}