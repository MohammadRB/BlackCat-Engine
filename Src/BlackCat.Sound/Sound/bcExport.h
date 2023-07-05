// [09/03/2022 MRB]

#pragma once

#ifdef BC_SOUND_DLL

#ifdef _MSC_VER
#define BC_SOUND_DLL __declspec(dllexport)
#endif

#else

#ifdef _MSC_VER
#define BC_SOUND_DLL __declspec(dllimport)
#endif

#endif
