// [08/05/2016 MRB]

#pragma once

#ifdef BC_BLACKCAT_DLL

#ifdef _MSC_VER

#define BC_BLACKCAT_DLL_EXP __declspec(dllexport)

#else

#endif

#else

#ifdef _MSC_VER

#define BC_BLACKCAT_DLL_EXP __declspec(dllimport)

#else

#endif

#endif

