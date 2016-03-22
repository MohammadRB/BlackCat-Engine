// [03/30/2015 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "Core/CorePCH.h"
#include "Platform/PlatformPCH.h"
#include "PlatformImp/PlatformImpPCH.h"

#if defined BC_GRAPHIC_DLL
#define BC_GRAPHIC_DLL_EXP __declspec(dllexport)
#else
#define BC_GRAPHIC_DLL_EXP __declspec(dllimport)
#endif