# Classes

Classes are fundermental in a language as C++, we use them a lot and they’re a good language feature to keep code structured and organised. 

Avoid arbitrary initialization on the body of a constructor if you can’t signal an error.

```cpp
    Foo::Foo()
    {
        initialize_something_that_can_fail(); // WRONG, NO WAY OF NOTIFYING FAILURE
    }

    int Foo::initialize() // OK, RETURN ERROR CODE ON FAILURE
    {
        int err = 0;
        
        err = initialize_something_that_can_fail();
        
        if (!err) 
        {
            return err;
        }
    }
```

For readability purposes, the initialize function returns an int. In a real-world example, this should return an enum value instead, more info is discussed bellow.

Avoid having objects who’s initialization can fail, as it’s easy to forget a call to or capturing the return value of the object’s “initialize” function.

If you really need to have an initialize function, try to use the following design

```cpp
    class Foo
    {
    public:
        static std::optional<Foo> create()
        {
            Foo foo;
            if (!foo.initialize())
            {
                REX_ASSERT("Failed to create Foo");
                return std::nullopt; // No object on invalid initialization
            }
            
            return foo;
        }
        
    private:
        Foo()
        {}
    }
```

An alternate way to guarantee an object initialises correctly is to use asserts directly in the constructor.

```cpp
    class Foo
    {
    public:
        Foo()
        {
            // do some initialization
            const bool some_bool_value = some_check();
            REX_ASSERT_X(some_bool_value, "Failed to initialize Foo");
        }
    }
```

## Implicit conversions

Avoid the possiblity of implicit conversions. When a constructor of an object only holds a single argument, mark that constructor explicit.

```cpp
    class Foo
    {
    public:
        Foo(int x) // WRONG, THIS ALLOWS IMPLICIT CONVERSIONS
        {}
    };

    class Bar
    {
    public:
        explicit Bar(int x) // OK, NO IMPLICIT CONVERSIONS
        {}
    };
```

## Copy and Moveable Types

We generally follow the rule of 0. This means, don’t define a constructor or destructor when they’re not needed.

When you do need a constructor or destructor, we follow the rule of 5. A copy constructor and move constructor is defaulted, unless you want custom behavior,
the same applies for assignment operators.

This is because it’s easy to forget to add a newly added member to the class’ copy or move constructor, let the compiler do its job and it’ll generate that for you. 
 
Note: be careful for shallow copies.
Note: do we need rule of 5 when defining a non-copy and non-move ctor?

## Structs vs Classes

Use a struct when you only want to carry data, everything else is a class. 

All fields in a struct must be public, no relationship between fields is allowed, constructor, destructor and helper methods may be present but aren’t required. 

If in doubt, use a class.

## Structs vs Pairs and Tuples

Prefer to use a named struct instead of a pair or a tuple to provide meaningful names to your fields. 

With C++17 and its structured binding, pairs and tuples have become more readable, however, you can use structured binding with custom defined structs as well which gives you the benefit of both.

## Inheritance

Compositions is often more appropriate than inhertiance. Avoid multiple inheritance as much as possible.

Don’t use protected data members, data members should always be private. Data methods may be protected.

Explictely annotate virtual functions with the ```virtual``` keyword, overriden functions only with the ```override``` keyword and final functions only with the ```final``` keyword. 

Prefer marking functions ```final``` instead of classes as this helps maintainability and readability as it’s easier to read over a class marked final than it is over a function.

Try to mark your function ```final``` as much as possible as this improves performance when the static type of your object is the class with the function marked final and it'll warn you when you're overwriting behavior which may not be your intend.

## Operator overloading

Only overload the operators if their meaning is obvious and unsurprising.
Never override the “address of” operator nor the comma operator.

## Declaration Order

Group definitions together, public comes first, then protected, then private.
Split methods and data members with another access specifier.
Debug member functions and fields come last in the class (not always possible for some fields), this is so they can easily be compiled out when grouped together.

**NEVER** delcare members protected or public of a class, create a "setter" or "getter" function when access is required.

Don't expose virtual member functions in base classes. What this means is, when a member function is protected in the base class, keep it private in the derived class.
C++ allows you to make member function public, even when they're protected in the base class, this however is not allowed in Rex.

What is allowed is the other way around, when a function is public in the base class, you are allowed to limit its access (by marking it protected or private) in the derived class.

When defining constructors, destructors and assignment operators, they’re always declared first.
The order is goes as followed:

```
  Default constructor (if any)          
  Other constructors   

  Copy constructor (if any)            
  Move constructor (if any)         

  Destructor                       

  Copy assignment operator (if any)    
  Move assignment operator (if any)   
```