# Templates

Avoid complicated template meta programming

Templates, although powerful, can make code quite unreadable if the hiearchy goes too deep.
Use templates where they’re really needed, eg. containers, utility types, rex::Nullptr.
Prefer overloading otherwise.

Always use ```typename``` over ```class``` when defining a template class or function.

```cpp
template <class T> // WRONG
void my_func()
{
    // ...
}

template <typename T> // OK
void my_func()
{
    // ...
}
```