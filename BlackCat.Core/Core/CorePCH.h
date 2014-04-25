#pragma once

#ifndef CorePCH_H
#define CorePCH_H

//// Add common controls 6.0 DLL to the manifest
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_IA64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif

//// Platform SDK defines, specifies that our min version is Windows Vista
//#ifndef WINVER
//#define WINVER 0x0600
//#endif
//
//#ifndef _WIN32_WINNT
//#define _WIN32_WINNT 0x0600
//#endif
//
//#ifndef _WIN32_WINDOWS
//#define _WIN32_WINDOWS 0x0410
//#endif
//
//#ifndef _WIN32_IE
//#define _WIN32_IE 0x0700
//#endif
//
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//#define STRICT							// Use strict declarations for Windows types

//// Windows Header Files:
//#include <windows.h>
//#include <commctrl.h>
//#include <psapi.h>

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//// C++ Standard Library Header Files
#include <functional>
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
#include <thread>

//// MSVC COM Support
//#include <comip.h>
//#include <comdef.h>

//// GDI+
//#include <gdiplus.h>

//// DirectX Includes
//#include "dxsdkver.h"
//#if (_DXSDK_PRODUCT_MAJOR < 9 || _DXSDK_BUILD_MAJOR < 1455)
//#error The installed DXSDK is out of date.
//#endif

//#ifdef _DEBUG
//#ifndef D3D_DEBUG_INFO
//#define D3D_DEBUG_INFO
//#endif
//#endif

//#include <dxgi.h>
//#include <d3d11.h>
//#include <d3dx11.h>
//#include <dxerr.h>
//#include <d3dx10.h>
//#include <D3Dcompiler.h>
//#include <d3d9.h>
//#include <D3d9types.h>
//#include <d3dx9.h>

//// XNA Math
//#include <Xnamath.h>

//// Static Lib Imports
//#pragma comment(lib, "dxerr.lib")
//#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "D3D9.lib")
//#pragma comment(lib, "D3D11.lib")
//#pragma comment(lib, "D3DX10.lib")
//#pragma comment(lib, "D3DX11.lib")
//#pragma comment(lib, "DXGI.lib")
//#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib, "psapi.lib")
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "gdiplus.lib")

//#ifdef _DEBUG
//#pragma comment(lib, "comsuppwd.lib")
//#pragma comment(lib, "d3dx9d.lib")
//#else
//#pragma comment(lib, "comsuppw.lib")
//#pragma comment(lib, "d3dx9.lib")
//#endif

// Warning disables
#pragma warning(disable : 4482)		// enum qualifier extension

// == BlackCat Definatoins ============================================================================ MRB ==

// == BlackCat Type Definations ======================================================================= MRB ==
#ifndef bcInline
#define bcInline __forceinline
#endif

#define asm _asm

#define bcTRUE true
#define bcFALSE false

#define bcNULL nullptr
#define bcNULLT std::nullptr_t

//typedef decltype(__nullptr) bcNULL;
typedef bool bcBOOL;
typedef char bcBYTE;
typedef char bcCHAR;
typedef wchar_t bcWCHAR;
typedef unsigned char bcUBYTE;
typedef short bcINT16;
typedef unsigned short bcUINT16;
typedef int bcINT32;
typedef unsigned int bcUINT32;
typedef unsigned int bcSIZE;
typedef unsigned long bcDWORD;
typedef float bcFLOAT32;
typedef long long bcINT64;
typedef unsigned long long bcUINT64;
typedef __nullterminated const char* bcLPCSTR;
typedef __nullterminated const wchar_t* bcLPCWSTR;

// == BlackCat General Definations ==================================================================== MRB ==
#ifdef WIN32
#define bcWIN32
#endif

//#ifdef _USRDLL
//#define BC_COREDLL
//#endif

#if defined BC_COREDLL
#define BC_COREDLL_EXP __declspec(dllexport)
#else
#define BC_COREDLL_EXP __declspec(dllimport)
#endif

// == BlackCat Debug Definations ====================================================================== MRB ==
#if defined( DEBUG ) || defined( _DEBUG )
#define BC_DEBUG
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
	_ASSERTE(((expr)))
#define bcStaticAssert(expr) static_assert(expr)
#else
#define bcAssert(expr) {}
#define bcStaticAssert {}
#endif

#define SAFE_RELEASE( x ) {if(x){(x)->Release();(x)=NULL;}}
#define SAFE_DELETE( x ) {if(x){delete (x);(x)=NULL;}}
#define SAFE_FREE( x ) {if(x){free(x);(x)=NULL;}}
#define SAFE_ALIGNED_FREE( x ) {if(x){_aligned_free (x);(x)=NULL;}}
#define SAFE_DELETE_ARRAY( x ) {if(x){delete[] (x);(x)=NULL;}}

// == BlackCat Memory Definations ===================================================================== MRB ==
#ifndef BC_MEMORY_ENABLE
#define BC_MEMORY_ENABLE
#endif

#ifdef BC_MEMORY_ENABLE

#ifdef BC_DEBUG
#ifndef BC_MEMORY_DEBUG
#define BC_MEMORY_DEBUG
#endif
#endif

//#ifndef BC_MEMORY_DEFRAG
//#define BC_MEMORY_DEFRAG
//#endif

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

// == BlackCat Container Definations ================================================================== MRB ==

#ifdef BC_DEBUG
#ifndef BC_CONTAINER_DEBUG
#define BC_CONTAINER_DEBUG
#endif
#endif

#endif
