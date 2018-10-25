// [ MRB]

#pragma once

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//// C++ Standard Library Header Files
#include <functional>
#include <numeric>
#include <array>
//#include <string>
//#include <vector>
//#include <list>
#include <memory>
//#include <map>
#include <cmath>
#include <sstream>
#include <fstream>
#include <istream>
#include <ostream>
#include <algorithm>
#include <chrono>
#include <type_traits>

//// MSVC COM Support
//#include <comip.h>
//#include <comdef.h>

// == BlackCat Definitions ================================================================================= /

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Utility/bcNoCopy.h"

// == BlackCat Memory Definitions ========================================================================= /
#ifndef BC_MEMORY_ENABLE
#define BC_MEMORY_ENABLE
#endif

#ifdef BC_MEMORY_ENABLE

#ifdef BC_DEBUG
#ifndef BC_MEMORY_DEBUG
#define BC_MEMORY_DEBUG
#endif
#endif

#ifndef BC_MEMORY_DEFRAG
#define BC_MEMORY_DEFRAG
#endif

#ifndef BC_MEMORY_MIN_ALIGN
#define BC_MEMORY_MIN_ALIGN 16
#endif

#ifndef BC_MEMORY_MAX_ALIGN
#define BC_MEMORY_MAX_ALIGN 128
#endif

//#ifndef BC_MEMORY_HANDLE_Ptr
//#define BC_MEMORY_HANDLE_Ptr
//#endif

#ifndef BC_MEMORY_TRACING
#define BC_MEMORY_TRACING
#endif

#ifdef BC_MEMORY_TRACING
#ifndef BC_MEMORY_LEAK_DETECTION
#define BC_MEMORY_LEAK_DETECTION
#endif
#endif

//#ifndef BC_MEMORY_ALIGN16
//#define BC_MEMORY_ALIGN16
//#endif

//#ifndef BC_MEMORY_AUTORESIZE_ALLOCATORS
//#define BC_MEMORY_AUTORESIZE_ALLOCATORS
//#endif

#endif

// == BlackCat Container Definations ======================================================================= /

#ifdef BC_DEBUG
#ifndef BC_CONTAINER_DEBUG
#define BC_CONTAINER_DEBUG
#endif
#endif
