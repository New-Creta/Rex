# Files

All files use snake_case.

```cpp
    my_header_file.h 
    my_source_file.cpp 
```
All header files end with a .h extension, all cpp files end with a .cpp extension.
Only templated code is allowed to be moved to a separate include file if this would improve readability. 
A template header file has the same name as the header file, but has the .template.h extension. 

```cpp
    my_header_file.template.h
```
## Header Files

Header files always start with a ``` #pragma once ``` statement and don’t use include guards. ``` #pragma once ``` is supported on all modern compilers and they’ll help code readability and has a little performance boost over include guards as well.

Header files should always compile on their own, this means that you should include all the required dependencies in a header file unless a forward declaration will due.

Forward declarations are preferred over inclusions as they keep header file size to a minimum, reducing the include graph and compile times.

### Inline functions
Inline functions aren’t really needed anymore on modern compilers, the compiler is smart enough to mark functions inline when needed.

However if you do need to specify an additional hint something needs to be inlined, used ``` __forceinline ``` over ```inline``` as the former will emit a warning when a function can’t be inlined while the latter will not.

## Source Files

It’s preferred not to use an open namespace in source files. Have your namespace added to your class scope or function. Because we use unity builds (also known as jumbo or combo builds), it’s not allowed to have global scope using-directives, neither in headers nor in source files.

when defining your class, prefer to use the following syntax:

```cpp

// SOURCE FILE

#include "foo.h"

namespace my_namespace // OPEN NAMESPACE
{
    foo::foo()
    {}
}

my_namespace::foo::foo() // PREFERED WAY OF DOING THINGS
{}

```

Namespace aliases however, are allowed in global scope in a cpp file as this should not be used elsewhere.


