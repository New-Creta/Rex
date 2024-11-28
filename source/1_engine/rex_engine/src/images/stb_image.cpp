#define STB_IMAGE_IMPLEMENTATION

#include "rex_engine/memory/global_allocator.h"

// Cannot implement the bellow until rex has a realloc implementation
//#define STBI_MALLOC(sz)           operator new(sz)
//#define STBI_REALLOC(p,newsz)     rex::realloc(p,newsz)
//#define STBI_FREE(p)              operator delete(p)

#include "rex_engine/images/internal/stb_image.h"