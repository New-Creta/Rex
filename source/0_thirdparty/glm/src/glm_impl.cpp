#include "glm/simd/platform.h"

#if GLM_COMPILER < 0x02000000 // MSVC
  #pragma warning(push)
  #pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#endif

#include "glm/glm.hpp"

#if GLM_COMPILER < 0x02000000 // MSVC
  #pragma warning(push)
#endif

// Nothing to implement