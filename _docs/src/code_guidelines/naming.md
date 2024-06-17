# Naming

**NEVER** use single letters as variable names, except in the syntax of a loop such as 

```cpp
    for(int i = 0; i < count; ++i)
    {
        // Loop body
    }
```

Put the & and * next to the type instead of next to the name of the variable, this will avoid warnings when the name of the variable is commented out when it’s an unused function argument

## Members

### In a class
Always written with snake case.

```
prefix: m_
int32 m_some_member_variable;
```
 
### In a struct
Always written with snake case.
Structs are meant to be used to wrap data and hold limited logic.
If you need to hide some members from a public interface, use a class.

```
prefix: none
int32 some_member_variable;
```

### Member Types
Always written with snake case.

```
prefix: none
using value_type = T;
```
 
## Locals

Always written with snake_case.

```
prefix: none
int32 some_local_variable;
```

## Static Variables

Always written with snake_case

```
prefix: s_
int32 s_some_static_variable;
```
 
## Globals

Try to avoid globals as much as possible.
Always defined in snake case

```
prefix: g_
int32 g_my_global;
```
 
## Parameters

Always written in lowerCamelCase.

```
prefix: none
int32 someParameter;
```
 
## Methods

Are always defined in snake case.

```cpp
void some_method();
```
 
## Classes

Are always defined in upper camel case, spaces are defined with an underscore.

```cpp
class SomeClass;
```
 
## Structs

Are always defined in upper camel case, spaces are defined with an underscore.

```cpp
struct SomeStruct;
```

## Global or Static Constants

Are always defined in upper camel case, these take ownership, so prefixes for statics or globals are ignored

```
prefix: none
int32 SomeConstValue;
```

## Enumerations

Are always defined in upper camel case

```cpp
enum class SomeEnum;
```

Enum elements are upper camel case, to follow cpp core guidelines and avoid all caps names

```cpp
enum class SomeEnum
{
  	MyFirst,
   	MySecond,
   	MyThird
}
```

## Defines

Are always defined in all caps, spaces are defined with an underscore.

```
prefix: none
SOME_DEFINE
```
 
## Getters and Setters

Getters don’t have a prefix, setters use the prefix ``` set_ ```.
Treat  ``` get_ ``` as command argument, when you want something done, perform some calculations to get, receive something from an object.
A bool getter has the prefix ‘ ``` is_ ``` , ``` should_ ``` or  ``` has_ ``` depending on what sounds grammatically correct.

```cpp
class Foo
{
public:
    int int_member() const;
    void set_int_member(int newValue);
    bool is_null() const;
    bool should_update() const;
    bool has_elements() const;
        
private:
    int m_int_member;
};
```

## Unused Parameter or Variable

Both commenting out the param name or using REX_UNUSED are allowed to mark variables as unused.

```cpp
void my_function(REX_UNUSED(int /*int_param*/))
{
    // ...
}

void my_function()
{
    REX_UNUSED(int some_debug_int = 0);
}
```
