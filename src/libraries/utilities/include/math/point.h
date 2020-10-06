#pragma once

namespace rex
{
    template<typename T>
    struct BasePoint
    {
        //-------------------------------------------------------------------------
        BasePoint(T xPos = 0, T yPos = 0)
            :x(xPos)
            , y(yPos)
        {}

        T x;
        T y;
    };

    struct Point32 : public BasePoint<int32>
    {};
    struct Point64 : public BasePoint<int64>
    {};

    using Point = Point32;
}