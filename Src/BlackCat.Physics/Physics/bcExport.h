// [20/12/2016 MRB]

#pragma once

#ifdef BC_PHYSICS_DLL

#ifdef _MSC_VER

#define BC_PHYSICS_DLL __declspec(dllexport)

#endif

#else

#ifdef _MSC_VER

#define BC_PHYSICS_DLL __declspec(dllimport)

#endif

#endif