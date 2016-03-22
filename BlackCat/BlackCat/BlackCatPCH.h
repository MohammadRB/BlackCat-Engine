// [03/12/2016 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "Core/CorePCH.h"
#include "Platform/PlatformPCH.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "Graphic/GraphicPCH.h"
#include "GraphicImp/GraphicImpPCH.h"

#if defined BC_BLACKCAT_DLL
#define BC_BLACKCAT_DLL_EXP __declspec(dllexport)
#else
#define BC_BLACKCAT_DLL_EXP __declspec(dllimport)
#endif