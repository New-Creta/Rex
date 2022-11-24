// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: tuple.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/types.h"

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/remove_reference.h"
#include "rex_std/internal/type_traits/is_empty.h"
#include "rex_std/internal/type_traits/is_lvalue_reference.h"
#include "rex_std/internal/type_traits/add_lvalue_reference.h"
#include "rex_std/internal/type_traits/conditional.h"
#include "rex_std/internal/type_traits/add_const.h"
#include "rex_std/internal/type_traits/add_volatile.h"
#include "rex_std/internal/type_traits/add_cv.h"
#include "rex_std/internal/type_traits/is_assignable.h"
#include "rex_std/internal/type_traits/is_constructible.h"
#include "rex_std/internal/type_traits/negation.h"
#include "rex_std/internal/type_traits/enable_if.h"

#include "rex_std/internal/functional/reference_wrapper.h"

namespace rsl
{
    STATIC_WARNING("using rsl::tuple - this is deprecated, please use a named struct");
    // non-recursive tuple implementation based on libc++ tuple implementation and description at
    // http://mitchnull.blogspot.ca/2012/06/c11-tuple-implementation-details-part-1.html

    template <typename ... T>
    class tuple;

    template <count_t I, typename Tuple>
    struct tuple_element;

    template <count_t I, typename Tuple>
    using tuple_element_t = typename tuple_element<I, Tuple>::type;

    template <count_t I, typename Tuple>
    using const_tuple_element_t = conditional_t<
        is_lvalue_reference_v
        <
            tuple_element_t<I, Tuple>
        >,
        add_lvalue_reference_t
        <
            const remove_reference_t
            <
                tuple_element_t<I, Tuple>
            >
        >,
        const tuple_element_t<I, Tuple>>;

    template <count_t I, typename ... Ts>
    tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t);

    template <count_t I, typename ... Ts>
    const_tuple_element_t<I, tuple<Ts...>>& get(const tuple<Ts...>& t);

    template <count_t I, typename ... Ts>
    tuple_element_t<I, tuple<Ts...>>&& get(tuple<Ts...>&& t);

    template <typename T, typename ...Ts>
    T& get(tuple<Ts...>& t);

    template <typename T, typename ... Ts>
    const T& get(const tuple<Ts...>& t);

    template <typename T, typename ... Ts>
    T&& get(tuple<Ts...>&& t);

    // tuple_types helper
    template <typename... Ts> struct tuple_types {};

    // tuple_size helper
    template <typename T> class tuple_size {};
    template <typename T> class tuple_size<const T> : public tuple_size<T> {};
    template <typename T> class tuple_size<volatile T> : public tuple_size<T> {};
    template <typename T> class tuple_size<const volatile T> : public tuple_size<T> {};

    template <typename... Ts> class tuple_size<tuple_types<Ts...>> : public integral_constant<count_t, sizeof...(Ts)> {};
    template <typename... Ts> class tuple_size<tuple<Ts...>> : public integral_constant<count_t, sizeof...(Ts)> {};

    template <class T>
    count_t tuple_size_v = tuple_size<T>::value;

    namespace internal
    {
        template <typename TupleIndices, typename... Ts>
        struct TupleImpl;
    } // namespace internal

    template <typename Indices, typename... Ts>
    class tuple_size<internal::TupleImpl<Indices, Ts...>> : public integral_constant<count_t, sizeof...(Ts)>
    {
    };

    // tuple_element helper to be able to isolate a type given an index
    template <count_t I, typename T>
    struct tuple_element
    {
    };

    template <count_t I>
    struct tuple_element<I, tuple_types<>>
    {
    public:
        static_assert(I != I, "tuple_element index out of range");
    };

    template <typename H, typename... Ts>
    struct tuple_element<0, tuple_types<H, Ts...>>
    {
    public:
        using type = H;
    };

    template <count_t I, typename H, typename... Ts>
    struct tuple_element<I, tuple_types<H, Ts...>>
    {
    public:
        using type = tuple_element_t<I - 1, tuple_types<Ts...>>;
    };

    // specialization for tuple
    template <count_t I, typename... Ts>
    struct tuple_element<I, tuple<Ts...>>
    {
    public:
        using type = tuple_element_t<I, tuple_types<Ts...>>;
    };

    template <count_t I, typename... Ts>
    struct tuple_element<I, const tuple<Ts...>>
    {
    public:
        using type = typename add_const_t<tuple_element_t<I, tuple_types<Ts...>>>::type;
    };

    template <count_t I, typename... Ts>
    struct tuple_element<I, volatile tuple<Ts...>>
    {
    public:
        using type = typename add_volatile_t<tuple_element_t<I, tuple_types<Ts...>>>::type;
    };

    template <count_t I, typename... Ts>
    struct tuple_element<I, const volatile tuple<Ts...>>
    {
    public:
        using type = typename add_cv<tuple_element_t<I, tuple_types<Ts...>>>::type;
    };

    // specialization for TupleImpl
    template <count_t I, typename Indices, typename... Ts>
    struct tuple_element<I, internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, tuple<Ts...>>
    {
    };

    template <count_t I, typename Indices, typename... Ts>
    struct tuple_element<I, const internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, const tuple<Ts...>>
    {
    };

    template <count_t I, typename Indices, typename... Ts>
    struct tuple_element<I, volatile internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, volatile tuple<Ts...>>
    {
    };

    template <count_t I, typename Indices, typename... Ts>
    struct tuple_element<I, const volatile internal::TupleImpl<Indices, Ts...>> : public tuple_element<
        I, const volatile tuple<Ts...>>
    {
    };

    // attempt to isolate index given a type
    template <typename T, typename Tuple>
    struct tuple_index
    {
    };

    template <typename T>
    struct tuple_index<T, tuple_types<>>
    {
        using DuplicateTypeCheck = void;
        tuple_index() = delete; // tuple_index should only be used for compile-time assistance, and never be instantiated
        static const count_t index = 0;
    };

    template <typename T, typename... TsRest>
    struct tuple_index<T, tuple_types<T, TsRest...>>
    {
        using DuplicateTypeCheck = int32;
        // after finding type T in the list of types, try to find type T in TsRest.
        // If we stumble back into this version of tuple_index, i.e. type T appears twice in the list of types, then DuplicateTypeCheck will be of type int32, and the static_assert will fail.
        // If we don't, then we'll go through the version of tuple_index above, where all of the types have been exhausted, and DuplicateTypeCheck will be void.
        static_assert(is_void_v<typename tuple_index<T, tuple_types<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<count_t>()");

        static const count_t index = 0;
    };

    template <typename T, typename TsHead, typename... TsRest>
    struct tuple_index<T, tuple_types<TsHead, TsRest...>>
    {
        using DuplicateTypeCheck = typename tuple_index<T, tuple_types<TsRest...>>::DuplicateTypeCheck;
        static const count_t index = tuple_index<T, tuple_types<TsRest...>>::index + 1;
    };

    template <typename T, typename Indices, typename... Ts>
    struct tuple_index<T, internal::TupleImpl<Indices, Ts...>> : public tuple_index<T, tuple_types<Ts...>>
    {
    };

    template <typename First, typename Second>
    struct pair;

    namespace internal
    {
        // swallow
        //
        // Provides a vessel to expand variadic packs.
        //
        template <typename... Ts>
        void swallow(Ts&&...) {}


        // TupleLeaf
        //
        template <count_t I, typename ValueType, bool IsTupleEmpty = is_empty_v<ValueType>>
        class TupleLeaf;

        template <count_t I, typename ValueType, bool is_empty_v>
        void swap(TupleLeaf<I, ValueType, is_empty_v>& a, TupleLeaf<I, ValueType, is_empty_v>& b)
        {
            ::rsl::swap(a.getInternal(), b.getInternal());
        }

        template <count_t I, typename ValueType, bool is_empty_v>
        class TupleLeaf
        {
        public:
            TupleLeaf() : m_value() {}
            TupleLeaf(const TupleLeaf&) = default;
            TupleLeaf& operator=(const TupleLeaf&) = delete;

            // We shouldn't need this explicit constructor as it should be handled by the template below but OSX clang
            // is_constructible_v type trait incorrectly gives false for is_constructible_v<T&&, T&&>::value
            explicit TupleLeaf(ValueType&& v) : m_value(::rsl::move(v)) {}

            template <typename T, typename = typename enable_if_t<is_constructible_v<ValueType, T&&>::value>::type>
            explicit TupleLeaf(T&& t)
                : m_value(::rsl::forward<T>(t))
            {
            }

            template <typename T>
            explicit TupleLeaf(const TupleLeaf<I, T>& t)
                : m_value(t.getInternal())
            {
            }

            template <typename T>
            TupleLeaf& operator=(T&& t)
            {
                m_value = ::rsl::forward<T>(t);
                return *this;
            }

            int32 swap(TupleLeaf& t)
            {
                ::rsl::internal::swap(*this, t);
                return 0;
            }

            ValueType& getInternal() { return m_value; }
            const ValueType& getInternal() const { return m_value; }

        private:
            ValueType m_value;
        };

        // TupleLeaf: Specialize for when ValueType is a reference 
        template <count_t I, typename ValueType, bool is_empty_v>
        class TupleLeaf<I, ValueType&, is_empty_v>
        {
        public:
            TupleLeaf(const TupleLeaf&) = default;
            TupleLeaf& operator=(const TupleLeaf&) = delete;

            template <typename T, typename = typename enable_if_t<is_constructible_v<ValueType, T&&>::value>::type>
            explicit TupleLeaf(T&& t)
                : m_value(::rsl::forward<T>(t))
            {
            }

            explicit TupleLeaf(ValueType& t) : m_value(t)
            {
            }

            template <typename T>
            explicit TupleLeaf(const TupleLeaf<I, T>& t)
                : m_value(t.getInternal())
            {
            }

            template <typename T>
            TupleLeaf& operator=(T&& t)
            {
                m_value = ::rsl::forward<T>(t);
                return *this;
            }

            int32 swap(TupleLeaf& t)
            {
                ::rsl::internal::swap(*this, t);
                return 0;
            }

            ValueType& getInternal() { return m_value; }
            const ValueType& getInternal() const { return m_value; }

        private:
            ValueType& m_value;
        };

        // TupleLeaf: partial specialization for when we can use the Empty Base Class Optimization
        template <count_t I, typename ValueType>
        class TupleLeaf<I, ValueType, true> : private ValueType
        {
        public:
            // true_type / false_type constructors for case where ValueType is default constructible and should be value
            // initialized and case where it is not
            TupleLeaf(const TupleLeaf&) = default;

            template <typename T, typename = typename enable_if_t<is_constructible_v<ValueType, T&&>::value>::type>
            explicit TupleLeaf(T&& t)
                : ValueType(::rsl::forward<T>(t))
            {
            }

            template <typename T>
            explicit TupleLeaf(const TupleLeaf<I, T>& t)
                : ValueType(t.getInternal())
            {
            }

            template <typename T>
            TupleLeaf& operator=(T&& t)
            {
                ValueType::operator=(::rsl::forward<T>(t));
                return *this;
            }

            int32 swap(TupleLeaf& t)
            {
                ::rsl::internal::swap(*this, t);
                return 0;
            }

            ValueType& getInternal() { return static_cast<ValueType&>(*this); }
            const ValueType& getInternal() const { return static_cast<const ValueType&>(*this); }

        private:
            TupleLeaf& operator=(const TupleLeaf&) = delete;
        };



        // MakeTupleTypes
        //
        //
        template <typename tuple_types, typename Tuple, count_t Start, count_t End>
        struct MakeTupleTypesImpl;

        template <typename... Types, typename Tuple, count_t Start, count_t End>
        struct MakeTupleTypesImpl<tuple_types<Types...>, Tuple, Start, End>
        {
            using TupleType = typename remove_reference_t<Tuple>::type;
            using type = typename MakeTupleTypesImpl<
                tuple_types<Types..., typename conditional<is_lvalue_reference<Tuple>::value,
                // append ref if tuple is ref
                tuple_element_t<Start, TupleType>&,
                // append non-ref otherwise
                tuple_element_t<Start, TupleType>>::type>,
                Tuple, Start + 1, End>::type;
        };

        template <typename... Types, typename Tuple, count_t End>
        struct MakeTupleTypesImpl<tuple_types<Types...>, Tuple, End, End>
        {
            using type = tuple_types<Types...>;
        };

        template <typename Tuple>
        using MakeTupleTypes_t = typename MakeTupleTypesImpl<tuple_types<>, Tuple, 0,
            tuple_size<typename remove_reference_t<Tuple>::type>::value>::type;


        // TupleImpl
        //
        //
        template <count_t I, typename Indices, typename... Ts>
        tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t);

        template <count_t I, typename Indices, typename... Ts>
        const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t);

        template <count_t I, typename Indices, typename... Ts>
        tuple_element_t<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t);

        template <typename T, typename Indices, typename... Ts>
        T& get(TupleImpl<Indices, Ts...>& t);

        template <typename T, typename Indices, typename... Ts>
        const T& get(const TupleImpl<Indices, Ts...>& t);

        template <typename T, typename Indices, typename... Ts>
        T&& get(TupleImpl<Indices, Ts...>&& t);

        template <count_t... Indices, typename... Ts>
        struct TupleImpl<integer_sequence<count_t, Indices...>, Ts...> : public TupleLeaf<Indices, Ts>...
        {
            TupleImpl() = default;

            // index_sequence changed to integer_sequence due to issues described below in VS2015 CTP 6. 
            // https://connect.microsoft.com/VisualStudio/feedback/details/1126958/error-in-template-parameter-pack-expansion-of-std-index-sequence
            // 
            template <typename... Us, typename... ValueTypes>
            explicit TupleImpl(integer_sequence<count_t, Indices...>, tuple_types<Us...>, ValueTypes&&... values)
                : TupleLeaf<Indices, Ts>(::rsl::forward<ValueTypes>(values))...
            {
            }

            template <typename OtherTuple>
            TupleImpl(OtherTuple&& t)
                : TupleLeaf<Indices, Ts>(::rsl::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...
            {
            }

            template <typename OtherTuple>
            TupleImpl& operator=(OtherTuple&& t)
            {
                swallow(TupleLeaf<Indices, Ts>::operator=(
                    ::rsl::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...);
                return *this;
            }

            TupleImpl& operator=(const TupleImpl& t)
            {
                swallow(TupleLeaf<Indices, Ts>::operator=(static_cast<const TupleLeaf<Indices, Ts>&>(t).getInternal())...);
                return *this;
            }

            void swap(TupleImpl& t) { swallow(TupleLeaf<Indices, Ts>::swap(static_cast<TupleLeaf<Indices, Ts>&>(t))...); }
        };

        template <count_t I, typename Indices, typename... Ts>
        tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t)
        {
            using Type = tuple_element_t<I, TupleImpl<Indices, Ts...>>;
            return static_cast<internal::TupleLeaf<I, Type>&>(t).getInternal();
        }

        template <count_t I, typename Indices, typename... Ts>
        const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t)
        {
            using Type = tuple_element_t<I, TupleImpl<Indices, Ts...>>;
            return static_cast<const internal::TupleLeaf<I, Type>&>(t).getInternal();
        }

        template <count_t I, typename Indices, typename... Ts>
        tuple_element_t<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t)
        {
            using Type = tuple_element_t<I, TupleImpl<Indices, Ts...>>;
            return static_cast<Type&&>(static_cast<internal::TupleLeaf<I, Type>&>(t).getInternal());
        }

        template <typename T, typename Indices, typename... Ts>
        T& get(TupleImpl<Indices, Ts...>& t)
        {
            using Index = tuple_index<T, TupleImpl<Indices, Ts...>>;
            return static_cast<internal::TupleLeaf<Index::index, T>&>(t).getInternal();
        }

        template <typename T, typename Indices, typename... Ts>
        const T& get(const TupleImpl<Indices, Ts...>& t)
        {
            using Index = tuple_index<T, TupleImpl<Indices, Ts...>>;
            return static_cast<const internal::TupleLeaf<Index::index, T>&>(t).getInternal();
        }

        template <typename T, typename Indices, typename... Ts>
        T&& get(TupleImpl<Indices, Ts...>&& t)
        {
            using Index = tuple_index<T, TupleImpl<Indices, Ts...>>;
            return static_cast<T&&>(static_cast<internal::TupleLeaf<Index::index, T>&>(t).getInternal());
        }


        // TupleLike
        //
        // type-trait that determines if a type is an ::rsl::tuple or an ::rsl::pair.
        //
        template <typename T> struct TupleLike : public false_type {};
        template <typename T> struct TupleLike<const T> : public TupleLike<T> {};
        template <typename T> struct TupleLike<volatile T> : public TupleLike<T> {};
        template <typename T> struct TupleLike<const volatile T> : public TupleLike<T> {};

        template <typename... Ts>
        struct TupleLike<tuple<Ts...>> : public true_type {};

        template <typename First, typename Second>
        struct TupleLike<::rsl::pair<First, Second>> : public true_type {};


        // TupleConvertible
        //
        //
        //
        template <bool IsSameSize, typename From, typename To>
        struct TupleConvertibleImpl : public false_type
        {
        };

        template <typename... FromTypes, typename... ToTypes>
        struct TupleConvertibleImpl<true, tuple_types<FromTypes...>, tuple_types<ToTypes...>>
            : public integral_constant<bool, conjunction<is_convertible<FromTypes, ToTypes>...>::value>
        {
        };

        template <typename From, typename To,
            bool = TupleLike<typename remove_reference_t<From>::type>::value,
            bool = TupleLike<typename remove_reference_t<To>::type>::value>
            struct TupleConvertible : public false_type
        {
        };

        template <typename From, typename To>
        struct TupleConvertible<From, To, true, true>
            : public TupleConvertibleImpl<tuple_size<typename remove_reference_t<From>::type>::value ==
            tuple_size<typename remove_reference_t<To>::type>::value,
            MakeTupleTypes_t<From>, MakeTupleTypes_t<To>>
        {
        };


        // TupleAssignable
        //
        //
        //
        template <bool IsSameSize, typename Target, typename From>
        struct TupleAssignableImpl : public false_type
        {
        };

        template <typename... TargetTypes, typename... FromTypes>
        struct TupleAssignableImpl<true, tuple_types<TargetTypes...>, tuple_types<FromTypes...>>
            : public bool_constant<conjunction<is_assignable<TargetTypes, FromTypes>...>::value>
        {
        };

        template <typename Target, typename From,
            bool = TupleLike<typename remove_reference_t<Target>::type>::value,
            bool = TupleLike<typename remove_reference_t<From>::type>::value>
            struct TupleAssignable : public false_type
        {
        };

        template <typename Target, typename From>
        struct TupleAssignable<Target, From, true, true>
            : public TupleAssignableImpl<
            tuple_size<typename remove_reference_t<Target>::type>::value ==
            tuple_size<typename remove_reference_t<From>::type>::value,
            MakeTupleTypes_t<Target>, MakeTupleTypes_t<From>>
        {
        };


        // TupleImplicitlyConvertible and TupleExplicitlyConvertible 
        //
        // helpers for constraining conditionally-explicit ctors
        //
        template <bool IsSameSize, typename TargetType, typename... FromTypes>
        struct TupleImplicitlyConvertibleImpl : public false_type
        {
        };


        template <typename... TargetTypes, typename... FromTypes>
        struct TupleImplicitlyConvertibleImpl<true, tuple_types<TargetTypes...>, FromTypes...>
            : public conjunction<is_constructible<TargetTypes, FromTypes>..., is_convertible<FromTypes, TargetTypes>...>
        {
        };

        template <typename TargetTupleType, typename... FromTypes>
        struct TupleImplicitlyConvertible
            : public TupleImplicitlyConvertibleImpl<
            tuple_size<TargetTupleType>::value == sizeof...(FromTypes),
            MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
        {
        };

        template<typename TargetTupleType, typename... FromTypes>
        using tuple_implicitly_convertible_t = enable_if_t<TupleImplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;

        template <bool IsSameSize, typename TargetType, typename... FromTypes>
        struct TupleExplicitlyConvertibleImpl : public false_type
        {
        };

        template <typename... TargetTypes, typename... FromTypes>
        struct TupleExplicitlyConvertibleImpl<true, tuple_types<TargetTypes...>, FromTypes...>
            : public conjunction<
            is_constructible<TargetTypes, FromTypes>...,
            negation<conjunction<is_convertible<FromTypes, TargetTypes>...>>>
        {
        };

        template <typename TargetTupleType, typename... FromTypes>
        struct TupleExplicitlyConvertible
            : public TupleExplicitlyConvertibleImpl<
            tuple_size<TargetTupleType>::value == sizeof...(FromTypes),
            MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
        {
        };

        template<typename TargetTupleType, typename... FromTypes>
        using tuple_explicitly_convertible_t = enable_if_t<TupleExplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;


        // TupleEqual
        //
        //
        //
        template <count_t I>
        struct TupleEqual
        {
            template <typename Tuple1, typename Tuple2>
            bool operator()(const Tuple1& t1, const Tuple2& t2)
            {
                static_assert(tuple_size<Tuple1>::value == tuple_size<Tuple2>::value, "comparing tuples of different sizes.");
                return TupleEqual<I - 1>()(t1, t2) && get<I - 1>(t1) == get<I - 1>(t2);
            }
        };

        template <>
        struct TupleEqual<0>
        {
            template <typename Tuple1, typename Tuple2>
            bool operator()(const Tuple1&, const Tuple2&)
            {
                return true;
            }
        };


        // TupleLess
        //
        //
        //
        template <count_t I>
        struct TupleLess
        {
            template <typename Tuple1, typename Tuple2>
            bool operator()(const Tuple1& t1, const Tuple2& t2)
            {
                static_assert(tuple_size<Tuple1>::value == tuple_size<Tuple2>::value, "comparing tuples of different sizes.");
                return TupleLess<I - 1>()(t1, t2) || (!TupleLess<I - 1>()(t2, t1) && get<I - 1>(t1) < get<I - 1>(t2));
            }
        };

        template <>
        struct TupleLess<0>
        {
            template <typename Tuple1, typename Tuple2>
            bool operator()(const Tuple1&, const Tuple2&)
            {
                return false;
            }
        };


        // MakeTupleReturnImpl
        //
        //
        //
        template <typename T> struct MakeTupleReturnImpl { using type = T; };
        template <typename T> struct MakeTupleReturnImpl<reference_wrapper<T>> { using type = T&; };

        template <typename T>
        using MakeTupleReturn_t = typename MakeTupleReturnImpl<decay_t<T>>::type;


        // tuple_cat helpers
        //
        //
        //

        // TupleCat2Impl
        template <typename Tuple1, typename Is1, typename Tuple2, typename Is2>
        struct TupleCat2Impl;

        template <typename... T1s, count_t... I1s, typename... T2s, count_t... I2s>
        struct TupleCat2Impl<tuple<T1s...>, index_sequence<I1s...>, tuple<T2s...>, index_sequence<I2s...>>
        {
            using ResultType = tuple<T1s..., T2s...>;

            template <typename Tuple1, typename Tuple2>
            static  ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
            {
                return ResultType(get<I1s>(::rsl::forward<Tuple1>(t1))..., get<I2s>(::rsl::forward<Tuple2>(t2))...);
            }
        };

        // TupleCat2
        template <typename Tuple1, typename Tuple2>
        struct TupleCat2;

        template <typename... T1s, typename... T2s>
        struct TupleCat2<tuple<T1s...>, tuple<T2s...>>
        {
            using Is1 = ::rsl::make_index_sequence<sizeof...(T1s)>;
            using Is2 = ::rsl::make_index_sequence<sizeof...(T2s)>;
            using TCI = TupleCat2Impl<tuple<T1s...>, Is1, tuple<T2s...>, Is2>;
            using ResultType = typename TCI::ResultType;

            template <typename Tuple1, typename Tuple2>
            static  ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
            {
                return TCI::DoCat2(::rsl::forward<Tuple1>(t1), ::rsl::forward<Tuple2>(t2));
            }
        };

        // TupleCat
        template <typename... Tuples>
        struct TupleCat;

        template <typename Tuple1, typename Tuple2, typename... TuplesRest>
        struct TupleCat<Tuple1, Tuple2, TuplesRest...>
        {
            using FirstResultType = typename TupleCat2<Tuple1, Tuple2>::ResultType;
            using ResultType = typename TupleCat<FirstResultType, TuplesRest...>::ResultType;

            template <typename TupleArg1, typename TupleArg2, typename... TupleArgsRest>
            static  ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2, TupleArgsRest&&... ts)
            {
                return TupleCat<FirstResultType, TuplesRest...>::DoCat(
                    TupleCat2<TupleArg1, TupleArg2>::DoCat2(::rsl::forward<TupleArg1>(t1), ::rsl::forward<TupleArg2>(t2)),
                    ::rsl::forward<TupleArgsRest>(ts)...);
            }
        };

        template <typename Tuple1, typename Tuple2>
        struct TupleCat<Tuple1, Tuple2>
        {
            using TC2 = TupleCat2<Tuple1, remove_reference_t<Tuple2>>;
            using ResultType = typename TC2::ResultType;

            template <typename TupleArg1, typename TupleArg2>
            static  ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2)
            {
                return TC2::DoCat2(::rsl::forward<TupleArg1>(t1), ::rsl::forward<TupleArg2>(t2));
            }
        };
    }  // namespace internal



    // tuple
    //
    // ::rsl::tuple is a fixed-size container of heterogeneous values. It is a
    // generalization of ::rsl::pair which hold only two heterogeneous values.
    //
    // https://en.cppreference.com/w/cpp/utility/tuple
    //
    template <typename... Ts>
    class tuple;

    template <typename T, typename... Ts>
    class tuple<T, Ts...>
    {
    public:
        tuple() = default;

        template <typename T2 = T,
            internal::tuple_implicitly_convertible_t<tuple, const T2&, const Ts&...> = 0>
            tuple(const T& t, const Ts&... ts)
            : mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, internal::MakeTupleTypes_t<tuple>{}, t, ts...)
        {
        }

        template <typename T2 = T,
            internal::tuple_explicitly_convertible_t<tuple, const T2&, const Ts&...> = 0>
            explicit  tuple(const T& t, const Ts&... ts)
            : mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, internal::MakeTupleTypes_t<tuple>{}, t, ts...)
        {
        }

        template <typename U, typename... Us,
            internal::tuple_implicitly_convertible_t<tuple, U, Us...> = 0>
            tuple(U&& u, Us&&... us)
            : mImpl(make_index_sequence<sizeof...(Us) + 1>{}, internal::MakeTupleTypes_t<tuple>{}, ::rsl::forward<U>(u),
                ::rsl::forward<Us>(us)...)
        {
        }

        template <typename U, typename... Us,
            internal::tuple_explicitly_convertible_t<tuple, U, Us...> = 0>
            explicit  tuple(U&& u, Us&&... us)
            : mImpl(make_index_sequence<sizeof...(Us) + 1>{}, internal::MakeTupleTypes_t<tuple>{}, ::rsl::forward<U>(u),
                ::rsl::forward<Us>(us)...)
        {
        }

        template <typename OtherTuple,
            typename enable_if_t<internal::TupleConvertible<OtherTuple, tuple>::value, bool>::type = false>
            tuple(OtherTuple&& t)
            : mImpl(::rsl::forward<OtherTuple>(t))
        {
        }

        template <typename OtherTuple,
            typename enable_if_t<internal::TupleAssignable<tuple, OtherTuple>::value, bool>::type = false>
            tuple& operator=(OtherTuple&& t)
        {
            mImpl.operator=(::rsl::forward<OtherTuple>(t));
            return *this;
        }

        void swap(tuple& t) { mImpl.swap(t.mImpl); }

    private:
        using Impl = internal::TupleImpl<make_index_sequence<sizeof...(Ts) + 1>, T, Ts...>;
        Impl mImpl;

        template <count_t I, typename... Ts_>
        friend tuple_element_t<I, tuple<Ts_...>>& get(tuple<Ts_...>& t);

        template <count_t I, typename... Ts_>
        friend const_tuple_element_t<I, tuple<Ts_...>>& get(const tuple<Ts_...>& t);

        template <count_t I, typename... Ts_>
        friend tuple_element_t<I, tuple<Ts_...>>&& get(tuple<Ts_...>&& t);

        template <typename T_, typename... ts_>
        friend T_& get(tuple<ts_...>& t);

        template <typename T_, typename... ts_>
        friend const T_& get(const tuple<ts_...>& t);

        template <typename T_, typename... ts_>
        friend T_&& get(tuple<ts_...>&& t);
    };

    // template specialization for an empty tuple
    template <>
    class tuple<>
    {
    public:
        void swap(tuple&) {}
    };

    template <count_t I, typename... Ts>
    tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t)
    {
        return get<I>(t.mImpl);
    }

    template <count_t I, typename... Ts>
    const_tuple_element_t<I, tuple<Ts...>>& get(const tuple<Ts...>& t)
    {
        return get<I>(t.mImpl);
    }

    template <count_t I, typename... Ts>
    tuple_element_t<I, tuple<Ts...>>&& get(tuple<Ts...>&& t)
    {
        return get<I>(::rsl::move(t.mImpl));
    }

    template <typename T, typename... Ts>
    T& get(tuple<Ts...>& t)
    {
        return get<T>(t.mImpl);
    }

    template <typename T, typename... Ts>
    const T& get(const tuple<Ts...>& t)
    {
        return get<T>(t.mImpl);
    }

    template <typename T, typename... Ts>
    T&& get(tuple<Ts...>&& t)
    {
        return get<T>(::rsl::move(t.mImpl));
    }

    template <typename... Ts>
    void swap(tuple<Ts...>& a, tuple<Ts...>& b)
    {
        a.swap(b);
    }


    // tuple operators
    //
    //
    template <typename... T1s, typename... T2s>
    bool operator==(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
    {
        return internal::TupleEqual<sizeof...(T1s)>()(t1, t2);
    }

    template <typename... T1s, typename... T2s>
    bool operator<(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
    {
        return internal::TupleLess<sizeof...(T1s)>()(t1, t2);
    }

    template <typename... T1s, typename... T2s>  bool operator!=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t1 == t2); }
    template <typename... T1s, typename... T2s>  bool operator> (const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return t2 < t1; }
    template <typename... T1s, typename... T2s>  bool operator<=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t2 < t1); }
    template <typename... T1s, typename... T2s>  bool operator>=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t1 < t2); }


    // tuple_cat 
    //
    //
    template <typename... Tuples>
    typename internal::TupleCat<Tuples...>::ResultType tuple_cat(Tuples&&... ts)
    {
        return internal::TupleCat<Tuples...>::DoCat(::rsl::forward<Tuples>(ts)...);
    }


    // make_tuple 
    //
    //
    template <typename... Ts>
    tuple<internal::MakeTupleReturn_t<Ts>...> make_tuple(Ts&&... values)
    {
        return tuple<internal::MakeTupleReturn_t<Ts>...>(::rsl::forward<Ts>(values)...);
    }


    // forward_as_tuple 
    //
    //
    template <typename... Ts>
    tuple<Ts&&...> forward_as_tuple(Ts&&... ts) 
    {
        return tuple<Ts&&...>(::rsl::forward<Ts&&>(ts)...);
    }


    // ignore 
    //
    // An object of unspecified type such that any value can be assigned to it with no effect.
    //
    // https://en.cppreference.com/w/cpp/utility/tuple/ignore
    //
    namespace internal
    {
        struct ignore_t
        {
            ignore_t() = default;

            template <typename T>
            const ignore_t& operator=(const T&) const
            {
                return *this;
            }
        };
    }// namespace internal

    static const internal::ignore_t ignore;


    // tie 
    //
    // Creates a tuple of lvalue references to its arguments or instances of ::rsl::ignore.
    //
    // https://en.cppreference.com/w/cpp/utility/tuple/tie
    //
    template <typename... Ts>
    tuple<Ts&...> tie(Ts&... ts) 
    {
        return tuple<Ts&...>(ts...);
    }


    // apply
    //
    // Invoke a callable object using a tuple to supply the arguments. 
    //
    // http://en.cppreference.com/w/cpp/utility/apply
    //
    namespace detail
    {
        template <class F, class tuple, count_t... I>
        decltype(auto) apply_impl(F&& f, tuple&& t, index_sequence<I...>)
        {
            return invoke(::rsl::forward<F>(f), get<I>(::rsl::forward<tuple>(t))...);
        }
    } // namespace detail

    template <class F, class tuple>
    decltype(auto) apply(F&& f, tuple&& t)
    {
        return detail::apply_impl(::rsl::forward<F>(f), ::rsl::forward<tuple>(t),
            make_index_sequence<tuple_size_v<remove_reference_t<tuple>>>{});
    }

#ifdef REX_USE_REX_CODING_GUIDELINES_FOR_RSL
    template <typename ... Ts>
    using Tuple = tuple<Ts...>;

    template <count_t I, typename Tuple>
    using TupleElementStruct = tuple_element<I, Tuple>;

    template <count_t I, typename Tuple>
    using TupleElement = TupleElementStruct<I, Tuple>;
    
    template <typename T, typename Tuple>
    using TupleIndex = tuple_index<T, Tuple>;

    template <typename T> 
    using TupleSizeStruct = tuple_size<T>;

    template <typename T>
    count_t TupleSize = TupleSizeStruct<T>::value;
#endif
}
