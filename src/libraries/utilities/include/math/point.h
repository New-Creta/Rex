#pragma once



namespace rex
{
    struct Point
    {
        //-------------------------------------------------------------------------
        Point(int32 xPos = 0, int32 yPos = 0)
            :x(xPos)
            ,y(yPos)
        {}

        int32 x;
        int32 y;
    };
}