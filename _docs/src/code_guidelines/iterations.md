# Iterations

When writing iterators, we always write the curly brackets ‘{‘, ‘}’ even when there’s only one line of code underneath the statement.
Always write code so it can exit as soon as possible.
Curly brackets are always placed on a single line **NEVER** on the same line as the statement.

```cpp
void my_function()
{
    for (int i = 0; i < 10'000; ++i)
    {
        if (condition)
            continue;
        
        do_something(i);
    }
}
```

Use range based for loops when iterating over a container, this is equally or more performant than any other for loop 99% of the time and makes it clear you’ll be iterating over all elements.
When you need an index, use a traditional for loop.

```cpp
for (auto it = my_vec.begin(); it != my_vec.end(); ++it) // WRONG
{
    do_something(*it);
}

for (int i : my_vec) // OK
{
    do_something(i);
}

for (int i = 0; i < string_vec.size(); ++i) // ALSO OK
{
    std::string& str = string_vec[i];
    
    do_something(i, str);
}
```