// [07/13/2016 MRB]

#pragma once

#ifdef BC_GRAPHIC_DLL

#ifdef _MSC_VER

#define BC_GRAPHIC_DLL __declspec(dllexport)
#define BC_GRAPHICSHARED_DLL __declspec(dllexport)

#else

#endif

#else

#ifdef _MSC_VER

#define BC_GRAPHIC_DLL __declspec(dllimport)
#define BC_GRAPHICSHARED_DLL __declspec(dllimport)

#else

#endif

#endif
