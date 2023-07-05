// [08/12/2016 MRB]

#pragma once

#ifdef BC_PHYSICSIMP_DLL

#ifdef _MSC_VER

#define BC_PHYSICSIMP_DLL __declspec(dllexport)

#else
#endif

#else

#ifdef _MSC_VER

#define BC_PHYSICSIMP_DLL __declspec(dllimport)

#else
#endif

#endif