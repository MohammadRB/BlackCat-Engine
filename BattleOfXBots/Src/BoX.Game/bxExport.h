// [15/02/2022 MRB]

#pragma once

#ifdef BX_GAME_DLL

#ifdef _MSC_VER

#define BX_GAME_DLL __declspec(dllexport)
#else
#endif

#else

#ifdef _MSC_VER

#define BX_GAME_DLL __declspec(dllimport)
#else
#endif

#endif