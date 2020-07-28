#pragma once



#include <type_traits>

namespace rex
{
    template<typename T>
    struct Bounds
    {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types can be used in Bounds class.");

    public:
        //-------------------------------------------------------------------------
        Bounds()
            : left(static_cast<T>(0))
            , right(static_cast<T>(0))
            , top(static_cast<T>(0))
            , bottom(static_cast<T>(0))
        {}

        //-------------------------------------------------------------------------
        Bounds(T t, T l, T b, T r)
            :top(t)
            ,left(l)
            ,bottom(b)
            ,right(r)
        {}

        //-------------------------------------------------------------------------
        bool operator ==(const Bounds& ref) const
        {
            return this->left == ref.left && this->right == ref.right && this->top == ref.top && this->bottom == ref.bottom;
        }
        //-------------------------------------------------------------------------
        bool operator!=(const Bounds& ref)
        {
            return this->left != ref.left || this->right != ref.right || this->top != ref.top || this->bottom != ref.bottom;
        }

        //-------------------------------------------------------------------------
        T width() const
        {
            return abs(right - left);
        }
        //-------------------------------------------------------------------------
        T height() const
        {
            return abs(bottom - top);
        }

        T left;
        T right;
        T top;
        T bottom;
    };

    using BoundsI = Bounds<int32>;
    using BoundsF = Bounds<float>;
    using BoundsL = Bounds<long>;
}