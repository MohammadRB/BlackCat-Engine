// [13/07/2016 MRB]

#pragma once

#include "Platform/bcExport.h"

#ifdef BC_PLATFORMIMP_DLL

#ifdef _MSC_VER

#define BC_PLATFORMIMP_DLL __declspec(dllexport)
#define BC_PLATFORMSHARED_DLL __declspec(dllexport)

#endif

#else

#ifdef _MSC_VER

#define BC_PLATFORMIMP_DLL __declspec(dllimport)
#define BC_PLATFORMSHARED_DLL __declspec(dllimport)

#endif

#endif
