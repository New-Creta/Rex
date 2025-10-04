#define STB_IMAGE_IMPLEMENTATION

#include "rex_engine/memory/memory.h"

#define STBI_MALLOC(sz)           rex::alloc(sz)
#define STBI_REALLOC(p,newsz)     rex::realloc(p,newsz)
#define STBI_FREE(p)              rex::dealloc(p)

#include "rex_engine/serialization/stb_image.h"