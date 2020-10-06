#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <typeinfo>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "rex_debug.h"

#include "helpers/defines.h"
#include "helpers/types.h"

#include <glm/glm.hpp>

#ifdef _WINDOWS
	#include <Windows.h>
#endif

#ifdef _OPENGL
	#include <GL/glew.h>
	#include <GL/GL.h>
	#include <GL/GLU.h>
#endif