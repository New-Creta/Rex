# Functions

A function can only take input arguments, all output must be performed through the return value, unless the function does not initialize the output variable but only adjusts it.
This is because of optimization where you don’t want to necessarily copy an object which might be expensive.

## Declaration

Returned variables are preferred because they improve readability and an object’s initialization is better to look for if it’s initialized through a return value than through an output parameter.
When using an output parameter, use the ``` rex::Out<T> ``` class to make it clear at call site that the object will change in the function.

When combining input with output parameters, put input parameters first and output parameters last.
Boolean arguments are forbidden as it’s too easy to pass in a literal for them. Use a yes-no enum instead. Other arithmetic types and pointers can be passed as literals as well, they are allowed but discouraged to use. Declare your function with ``` rex::NoLiteral ``` or call them with ``` rex::NullPtr<T> ``` to improve readability.

```cpp
    DEFINE_YES_NO_ENUM(ShouldDoSomething);

    void foo1(ShouldDoSomething shouldDoSomething)
    {
        if (shouldDoSomething)
        {
            // do something
        }
    }

    void foo2(rex::NoLiteral<int> x)
    {
        // do something
    }

    void foo3(rex::Nullptr<int> p)
    {
        // do something
    }

    void bar()
    {
        //foo1(true); // does not compile
        //foo2(1); // does not compile
        //foo3(nullptr); // compiles, but is not recommended
        
        int x = 0;
        
        foo1(ShouldDoSomething::Yes); // OK
        foo2(x); // OK
        foo3(rex::Nullptr<int>); // OK
    }
```

## Definition

Write short functions that self comment what they do. it's recommended to not make your functions too long (50+ lines).
Your function should state clearly what’s being done, if you find yourself having to comment different parts of the function, wrap that part into its own function and call that instead.

## Overloading

Prefer overloading over default arguments, but be smart when doing so. Eg. when you want to overload for ```const char*``` and ```std::string```, overload for ```std::string_view``` instead.
Default arguments are discouraged for usage and are **forbidden** when using virtual functions.

## Trailing return types

Trailing return types are only allowed when the ordinary syntax is impractical or make the code much less readable.
There’s only 1 possible case where a trailing return type is required

```cpp
void my_function()
{
    auto my_lambda = [](int& x) /* ->int* */
    {
        if (x == 0)
        {
            return &x;
        }
        
        return nullptr; // Error: Cannot convert std::nullptr_t to int*
    };
    
    auto my_lambda_2 = [](int& x)
    {
        return x == 0
        ? &x
        : nullptr; // OK
    };
}
```

## Virtual functions

Virtual functions can only have default behaviour that must be overriden. It's a bad design to call a base virtual function from a derived one. 
This makes it unclear when the base virtual function should be called and makes debugging harder.
Instead, make the base's default function call another private function that does what you say it should, and call that one from the derived, making it more clear when that should be called as well.

```cpp
    class Base
    {
    public:
    virtual void do_something()
    {
        // do something
    }
    };

    class Derived : public Base
    {
    public:
    void do_something() override
    {
        // do something else first

        Base::do_something(); // BAD!
    }
    };

    class Base2
    {
    public:
    virtual void do_something()
    {
        do_something_impl();
    }

    protected:
    void do_something_impl()
    {
        // do something
    }
    };

    class Derived2 : public Base2
    {
    void do_something() override
    {
        // do something else first

        do_something_impl(); // GOOD!
    }
    };
```

If however, some code always needs to be executed before and/or after the virtual call, use the following design.

```cpp
    class Base
    {
    public:
        void do_something()
        {
            pre_do_something();		// makes it easy to call this before every virtual call
            do_something_impl();
            post_do_something();	// makes it easy to call this after every virtual call
        }
        
    protected:
        virtual void do_something_impl() = 0;
        
    private:
        void pre_do_something();
        void post_do_something();
    };

    class Derived : public Base
    {
    protected:
        void do_something_impl() override
        {
            // Do something
        }
        
    };
```

With this design, the developer of a derived class is not responsible of calling the base pre and post functions.
This avoids bugs for not calling such functions and is easy to debug and track down as well, keeping responsibility to the base class.
