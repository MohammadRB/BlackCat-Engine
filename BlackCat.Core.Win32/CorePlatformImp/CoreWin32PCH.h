// [10/11/2014 MRB]

#pragma once

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
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX						// Suppress min-max macroes
//#define STRICT							// Use strict declarations for Windows types

// Windows Header Files:
#include <windows.h>
//#include <Synchapi.h>
//#include <commctrl.h>
//#include <psapi.h>

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"

#if defined BC_COREWIN32_DLL
#define BC_COREWIN32_DLL_EXP __declspec(dllexport)
#else
#define BC_COREWIN32_DLL_EXP __declspec(dllimport)
#endif