// [10/08/2014 MRB]

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cstddef>
#include <type_traits>
#include <functional>
#include <math.h>
#include <iterator>
#include <limits>
#include <utility>
#include <chrono>
#include <assert.h>

// == BlackCat Definitions ==================================================================================== /

#ifndef bcInline
#define bcInline inline
#endif

#ifndef asm
#define asm _asm
#endif

// == BlackCat Debug Definitions ============================================================================== /

#if defined( DEBUG ) || defined( _DEBUG )
#ifndef BC_DEBUG
#define BC_DEBUG
#endif
#include <crtdbg.h>
#endif

#ifdef BC_DEBUG
#ifndef BC_ASSERTION
#define BC_ASSERTION
#endif
#endif

#ifdef BC_ASSERTION
#define BC_DEBUG_BREAK() asm { int 3 }
#define BC_ASSERT(expr) assert(((expr)))
#else
#define bcAssert(expr) void(0)
#endif

#ifdef UNICODE
#define BC_UNICODE
#endif

// == BlackCat General Definitions ============================================================================ /