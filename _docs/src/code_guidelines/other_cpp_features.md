# Other C++ Features

## Exceptions

Exceptions are disabled over the entire code base. 

## Run-Time Type Information (RTTI)
RTTI is disabled over the entire code base.

## Casting
Prefer to use C++ cast over C-casts. Although they are more verbose, it makes them easier to track and the expected behavior is clearer. 

The REX engine also has numeric casts and a safe version of it. numeric cast is just an alias for static cast, the safe version performs checking if there's any loss in value after the cast. In retail builds, the safe overload is replaced by regular static casts for performance reasons.

```cpp
void func()
{
    uint32 uint_x = 10;
    int32 int_x = rex::safe_numeric_cast<int32>(uint_x);
}
```

## Use of const

prefer to use for all local variables (where possible), making it clear variables won't change.

Don’t cast away const through class getters. 

This means if you have a const getter, return a const pointer, const reference or return by value.
Arguments captured by value don’t require const.

```cpp
class Foo
{
public:
    //int* my_int() const // BAD, CAST AWAY CONST
    //{
    //    return m_my_int;
    //}

    const int* my_int() const // GOOD
    {
        return m_my_int;
    }

private:
    int* m_my_int;
}
```

if you do need to cast away const (eg. when interacting with a C API), wrap the const cast in its own API function.

```cpp
template <typename T>
T* dx_const_cast(const T* dx_object)
{
    return const_cast<T*>(dx_object);
}
```

We use west const const in the code base, this means const comes on the left hand side of the type.

## Use of constexpr

Use constexpr wherever possible, but don’t use constexpr to force inlining.
mark file scope constexpr variables inline to reduce binary size of the generated file by the compiler
mark block scope constexpr variables static. 

## Integer Types

Use signed integer for **ALL** variables that require arithmetic operations. Only use unsigned integer values for masks, bit patterns. Don’t use unsigned integers to represent a number. Don’t use unsigned integers to determine a number will never be negative, use assertions for this.

When writing large constants, use the ‘ to separate each group of 3 digits.

```cpp
constexpr int my_big_int = 10'000;
```

## Floating points

Always write a radix point, with digits on both sides. Readability is improved if all floating-point literals take this familiar form.
```cpp
constexpr float my_float = 1.0f;
```

## Preprocessor Macros

Avoid defining macros, especially in headers. Prefer functions, enum classes, const or constexpr variables. Macros have global scope, which make them a very good candidate of causing bugs.
Because we use unity builds, undef macros defined in a cpp file at the bottom of that cpp file, so they don't leak to other files

## 0, nullptr and NULL

Use nullptr for pointers that are null and ‘\0’ for chars.
When using WIN32 API functions, it’s allowed to pass in NULL.

## Sizeof

Use sizeof(var_name) over sizeof(var_type) where possible.
This makes sure the sizeof is updated appropriately if someone changes the variable type.

## Iterators
When passing in iterators to algorithms, be very clear which one you use. Iterators may be captured with auto, which in this case would keep their constness as it’s part of the type.
That’s why the following:

```cpp
auto it = std::find_if(my_vec.begin(), my_vec.end(),
[](int x)
{
    return x == 0;
});
```

is different from

```cpp
auto it = std::find_if(my_vec.cbegin(), my_vec.cend(),
[](int x)
{
    return x == 0;
});
```

The first iterator is non-const, so it’s allowed to change the underlying value, with the second iterator, this is not the case.

## Initializer list
The initializer lists in a constructor starts on a new line with each separation token at the front.

```cpp
Foo::Foo(int first_param, int second_param)
    : m_first_member(first_param)
    , m_second_member(second_param)
{}
```

