// [13/07/2016 MRB]

#pragma once

#ifdef BC_COREPLATFORMIMP_DLL

#ifdef _MSC_VER
#define BC_COREPLATFORMIMP_DLL __declspec(dllexport)
#else
#endif

#else

#ifdef _MSC_VER
#define BC_COREPLATFORMIMP_DLL __declspec(dllimport)
#else
#endif

#endif