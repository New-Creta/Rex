# Enumerations

## Scoped vs Unscoped enum

**ALWAYS** use enum classes, unscoped enums are a thing from the past and have a lot of downsides.

Scoped enums, aka enum classes, are type safe, you know which enum you're working with. 

The only downside from scoped is that you can't use them to index into an array, but this is a good thing and not a bad thing.

If you use an enum as an int, you depend on their internal order which is very likely to cause bugs (and can be hard to debug as well).
