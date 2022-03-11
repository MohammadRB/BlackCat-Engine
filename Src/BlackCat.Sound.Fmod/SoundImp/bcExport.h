// [07/13/2016 MRB]

#pragma once

#include "Sound/bcExport.h"

#ifdef  BC_SOUNDIMP_DLL

#ifdef _MSC_VER
#define BC_SOUNDIMP_DLL __declspec(dllexport)
#endif

#else

#ifdef _MSC_VER
#define BC_SOUNDIMP_DLL __declspec(dllimport)
#endif

#endif