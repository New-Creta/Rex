#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <typeinfo>
#include <chrono>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifdef _WINDOWS
    #include <Windows.h>
#endif

#include "rex_debug.h"

#include "helpers/defines.h"
#include "helpers/types.h"

#include "config/config.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>