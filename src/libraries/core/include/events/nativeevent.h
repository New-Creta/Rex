#pragma once

namespace rex
{
    namespace events
    {
        class NativeEvent
        {
        public:
            //-------------------------------------------------------------------------
            NativeEvent() = default;
            //-------------------------------------------------------------------------
            virtual ~NativeEvent() = default;

            //-------------------------------------------------------------------------
            virtual const void* getData() const = 0;
        };
    }
}