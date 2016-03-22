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

//// MSVC COM Support
//#include <comip.h>
//#include <comdef.h>

// == BlackCat Definatoins ================================================================================= /

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Memory/bcMemAlloc.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcFuture.h"
#include "CorePlatformImp/Concurrency/bcConditionVariable.h"
#include "CorePlatformImp/Concurrency/bcConcurrencyUtility.h"
#include "CorePlatformImp/Utility/bcClock.h"

// == BlackCat General Definations ========================================================================= /

#if defined BC_CORE_DLL
#define BC_COREDLL_EXP __declspec(dllexport)
#else
#define BC_COREDLL_EXP __declspec(dllimport)
#endif

// == BlackCat Memory Definations ========================================================================= /
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
