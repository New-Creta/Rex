# Debug vs Retail

## Functions

Some functions are only allowed in non retail builds.

This means they need to be compiled out. It's a common practice to compile out the body of these sort functions, however, this allows them to be called from retail builds which would still initialize the function arguments.

That's why we don't do this in **REX**, instead we compile out the entire function itself, function declaration and definition.
There are macros defined to make this a bit more readable, eg. DEBUG_ONLY(func), which strips out its argument in retail builds.

## Macro configuration

Often we use macros to enable/disable specific code paths.
Most often these code paths will be enabled/disabled using REX_RELEASE, however, it's not allowed to use REX_RELEASE to enable or disable code path directly.

Instead use a different macro that you do or do not define based if REX_RELEASE or any other macro is defined.
This makes it possible to override this rule for debugging purposes without affecting other code.

```cpp
void func()
{
    #ifdef REX_RELEASE
        log_to_console(); // BAD
    #endif
    }


    #ifdef REX_RELEASE
    #define REX_ENABLE_LOGGING
    #endif

    void func()
    {
    #ifdef REX_ENABLE_LOGGING
        log_to_console(); // GOOD
    #endif
}
```