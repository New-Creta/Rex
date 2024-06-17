# Selections

When writing statements, either if or else, we always write curly brackets ‘{‘, ‘}’ even when there’s only one statement underneath it.
The exception to this rule is early out statements. When an early out statement is used, you can leave out the curly brackets.
Always write code so it can exit as soon as possible.

Curly brackets are always placed on a single line **NEVER** on the same line as the statement.
Don’t put code in big if statements, the condition of the if statement should be readable throughout the entire body of the if statement.

```cpp
void my_function()
{
    if (condition) // EARLY OUT, NO '{' '}' REQUIRED
        return;
      
    if (some_other_condition)
    {
        do_something();
    }
    else if (some_other_condition)
    {
        do_something_else();
    }   
}
```
