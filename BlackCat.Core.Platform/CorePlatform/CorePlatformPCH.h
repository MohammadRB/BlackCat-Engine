// [10/08/2014 MRB]

#pragma once

#include <stdio.h>
#include <stdlib.h>
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

// == BlackCat Definatoins ==================================================================================== /

#ifndef bcInline
#define bcInline inline
#endif

#ifndef asm
#define asm _asm
#endif

#ifndef noexcept
#define noexcept(p) noexcept
#endif

//#ifndef except
//#define except throw(...)
//#endif

// == BlackCat Debug Definations ============================================================================== /

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
#define bcDebugBreak() asm { int 3 }
#define bcAssert(expr) \
	assert(((expr)))
#define bcStaticAssert(expr, msg) static_assert(expr, msg)
#else
#define bcAssert(expr) {}
#define bcStaticAssert(expr, msg) {}
#endif

// == BlackCat General Definations ============================================================================ /

#ifdef BC_COREPLATFORM_DLL
#define BC_COREPLATFORM_DLL_EXP __declspec(dllexport)
#else
#define BC_COREPLATFORM_DLL_EXP __declspec(dllimport)
#endif