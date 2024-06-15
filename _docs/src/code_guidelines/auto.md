# Auto

Don’t use auto when it’s not required.

Auto doesn’t references, which is easy to miss when reading or writing code and can hurt performance.
Auto can make your code unreadable when it’s not clear to the reader what the type actually is. Don’t write something like this:

```cpp
    auto result = do_something_special();
```

this makes it not clear to the reader what result is. auto is only allowed when the type is too long and when it’s clear.

```cpp
    auto start_time = std::high_resolution_clock::now();
    auto it = my_vec.begin();
```

