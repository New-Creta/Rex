# Scoping

Scoping is very important in a big code base. 
Have the proper scope for objects improves readability, usability, maintainability and will your make life easier in general.

## Global Scope

Nothing is allowed under global scope, always hide your functions, objects under at least 1 namespace.

## Namespaces

All code should be under a namespace, they will highly improve code readability and avoid clashing names.

Don’t use using-directives in headers, they may be allowed in source files only if there is absolutely no confusion.

Only use inline namespaces after clear review of no better alternative.

Using-directives are only allowd in local scope, and even in there, keep the scope to a minimum.  

```cpp
    namespace my_namespace
    {
        void namespace_function()
        {
            // Do something
        }
    }

    void wrong_function(int x) // WRONG
    {
        using namespace my_namespace;
        if (x == 0)
        {
            namespace_function();
        }   
    }

    void my_function_1(int x) // OK
    {
        if (x == 0)
        {
            using namespace my_namespace;
            namespace_function();
        }   
    }

    void my_function_2(int x) // EVEN BETTER
    {
        if (x == 0)
        {
            my_namespace::namespace_function();
        }   
    }

```
### Anonymous namespace

When definitions don’t need to be referenced outside of that file, place them in an unnamed namespace, declaring them static is an old way of doing things, we don’t do that anymore. Don’t use either of these constructs in a header file.

However, it is preferred to put these definitions in an open namespace of the namespace belonging to this file to improve readability.

```cpp

// SOURCE FILE - belonging to rex namespace

namespace // NOT RECOMMENDED
{
    int g_my_global_int = 0;
}

namespace rex // OK
{
    float g_my_global_float = 1.0f;
}

rex::foo()
{
    g_my_global_int = 1;
    g_my_global_float = 2.0f;
}

```

### Nonmember, static member and globals functions

Don’t use a class just to group static functions together. Use a namespace instead. The interface is the same and it’s clearer that an instance of it will never exist. When writing utility functions linked to a specific system (eg. file system) put the utility functions in a namespace.

```cpp

namespace rex::vfs
{
    void some_utility_function();
}

void rex::foo()
{
    rex::vfs::some_utility_function();
}
```

## Static and Global Variables
Objects with global static storage duration are forbidden unless they are trivially destructible. This means the destructor does not do anything, even taking member and base destructors into account.

Static or global variables should be const or constexpr by default, only in rare cases they are not. If your object uses dynamic initialization, wrap them in a function and return a local static instead.

```cpp
MyThing g_my_thing; // WRONG

MyThing& my_thing() // OK
{
    static MyThing my_thing;

    return my_thing;
}
```

Statics between translation units are initialized in an undefined order, so it’s discouraged to have them depend on each other. Statics are commonly string literals or math constants used in specific parts of the codebase.

A general rule of thumb: if you can’t mark your static as constexpr, it’s likely it uses dynamic initialization (before C++20) and is therefore not allowed to be a global static variable.

## Local Variables

Always initialize variables when they’re declared

```cpp
    void my_function()
    {
        int i;
        i = f(); // WRONG
        
        int i = f(); // OK
        
        std::vector<int> vec;
        vec.push_back(1);   // WRONG
        vec.push_back(2);   // WRONG
        
        std::vector<int> vec = {1, 2}; // OK
    }
```

A local variable must be put in the narrowest scope possible.

```cpp
    void my_function()
    {
        int i = 0;  // WRONG, NOT USED IN THIS SCOPE
        
        // Some code not using 'i'
        // ...
        
        if (condition)
        {
            i = do_something();
            do_something_with(i);
        }
        
        // i not used here
        // ...
    }

    void my_function()
    {
        // Some code
        // ...
        
        if (condition)
        {
            int i = do_something(); // OK, ONLY USED IN THIS SCOPE
            do_something_with(i);
        }
        
        // Some code
        // ...
    }
```

There’s 1 exception to this rule. When the variable is a class-type and it’s used in a loop, its constructor and destructor will be called every time when entering or leaving the scope. 

It’s okay to declare it right outside of the scope then. Smart compilers moved plain old datatypes out of a loop automatically but this is implementation defined, better to help the compiler instead of hoping it will do it for you.

Note: be careful this doesn’t change the behavior of the code.

```cpp
    void my_function()
    {
        for (int i = 0; i < 10'000; ++i)
        {
            Foo foo;    // WRONG, CTOR AND DTOR CALLED 10'000 TIMES
            foo.do_something();
        }
    }

    void my_function()
    {
        Foo foo;    // OK, CTOR AND DTOR ONLY CALLED ONCE
        for (int i = 0; i < 10'000; ++i)
        {
            foo.do_something();
        }
    }
```