# Good Practices

## LValue vs RValue

When passing in an object to a function and you don't want it to be changed, pass it by const ref.

If the object is small (sizeof(T) <= sizeof(void*)), you can pass it by value.

If you want to **move** the object, pass it by rvalue reference. **NEVER** pass it by lvalue reference if it'll be moved from.