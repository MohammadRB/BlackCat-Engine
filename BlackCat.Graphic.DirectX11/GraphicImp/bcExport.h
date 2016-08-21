// [07/13/2016 MRB]

#pragma once

#ifdef BC_GRAPHICIMP_DLL

#ifdef _MSC_VER

#define BC_GRAPHICIMP_DLL __declspec(dllexport)
//#ifdef BC_PLATFORMIMP_DLL
#define BC_GRAPHICSHARED_DLL __declspec(dllexport)
//#endif

#else

#endif

#else

#ifdef _MSC_VER

#define BC_GRAPHICIMP_DLL __declspec(dllimport)
//#ifdef BC_PLATFORMIMP_DLL
#define BC_GRAPHICSHARED_DLL __declspec(dllimport)
//#endif

#else

#endif

#endif