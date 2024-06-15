# Lambdas

Don’t use lambda’s when they’re not needed. They can be hard to track down where they’re declared when the user is expecting a function instead. 
Prefer using global functions in an anonymous namespace instead.
An inline lambda given as a callback to a function is always defined on the next line.

```cpp
    auto it = std::find_if(my_vec.cbegin(), my_vec.cend(),
        [](int x)
    {
        return x == 0;
    });
```

This keep the lambda definition together with its captures and parameters.
