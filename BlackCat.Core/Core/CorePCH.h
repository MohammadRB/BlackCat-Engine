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

//// Warning disables
//#pragma warning(disable : 4482)		// enum qualifier extension

// == BlackCat Definatoins ================================================================================= /

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/Memory/bcMemAlloc.h"

// == BlackCat General Definations ========================================================================= /

#if defined BC_CORE_DLL
#define BC_COREDLL_EXP __declspec(dllexport)
#else
#define BC_COREDLL_EXP __declspec(dllimport)
#endif

// == BlackCat Debug Definations ========================================================================== /

//#define SAFE_FREE( x ) {if(x){free(x);(x)=nullptr;}}
//#define SAFE_DELETE( x ) {if(x){delete (x);(x)=nullptr;}}
//#define SAFE_ALIGNED_FREE( x ) {if(x){_aligned_free (x);(x)=nullptr;}}
//#define SAFE_DELETE_ARRAY( x ) {if(x){delete[] (x);(x)=nullptr;}}
//#define SAFE_RELEASE( x ) {if(x){(x)->Release();(x)=nullptr;}}

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
