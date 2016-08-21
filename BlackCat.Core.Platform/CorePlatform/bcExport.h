// [07/13/2016 MRB]

#pragma once

#ifdef BC_COREPLATFORM_DLL

#ifdef _MSC_VER
#define BC_COREPLATFORM_DLL __declspec(dllexport)
#else
#endif

#else

#ifdef _MSC_VER
#define BC_COREPLATFORM_DLL __declspec(dllimport)
#else
#endif

#endif
