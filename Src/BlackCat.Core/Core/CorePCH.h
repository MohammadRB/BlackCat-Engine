// [ MRB]

#pragma once

// C RunTime Header Files
#include <cstdio>
#include <cstdlib>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ Standard Library Header Files
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <istream>
#include <memory>
#include <numeric>
#include <ostream>
#include <sstream>
#include <type_traits>

// == BlackCat Definitions ================================================================================= /

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/Utility/bcNoCopy.h"

// == BlackCat Memory Definitions ========================================================================= /

#ifndef BC_MEMORY_ENABLE
#define BC_MEMORY_ENABLE
#endif

#ifdef BC_MEMORY_ENABLE

#ifndef BC_MEMORY_DEFRAG
//#define BC_MEMORY_DEFRAG
#endif

#ifndef BC_MEMORY_MIN_ALIGN
#define BC_MEMORY_MIN_ALIGN 16
#endif

#ifndef BC_MEMORY_MAX_ALIGN
#define BC_MEMORY_MAX_ALIGN 128
#endif

#ifdef BC_DEBUG
#ifndef BC_MEMORY_DEBUG
#define BC_MEMORY_DEBUG
#endif
#ifndef BC_MEMORY_LEAK_DETECTION
#define BC_MEMORY_LEAK_DETECTION
#endif
#endif

#ifndef BC_MEMORY_TRACING
#define BC_MEMORY_TRACING
#endif

#endif

// == BlackCat Container Definitions ======================================================================= /

#ifdef BC_DEBUG
#ifndef BC_CONTAINER_DEBUG
#define BC_CONTAINER_DEBUG
#endif
#endif
