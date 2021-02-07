 // [10/16/2016 MRB]

/*
 * This file is created duo to circular dependency between 
 * bcScriptRuntime.h -> bcScriptExternalObject.h -> bcScriptPrototype.h -> bcScriptContext.h -> bcScriptRuntime.h
 */

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Platform/Script/bcScriptRuntime.h"
#include "Platform/Script/bcScriptExternalObject.h"

#pragma once

namespace black_cat
{
	namespace platform
	{
		template< core_platform::bc_platform TPlatform >
		template< typename T, typename ...TA >
		bc_script_external_object<T>* bc_platform_script_runtime< TPlatform >::create_native(TA&&... p_args)
		{
			return BC_NEW(bc_script_external_object<T>(T(std::forward< TA >(p_args)...)), core::bc_alloc_type::unknown);
		}

		template< core_platform::bc_platform TPlatform >
		template< typename T >
		void bc_platform_script_runtime<TPlatform>::destroy_native(bc_script_external_object<T>* p_object)
		{
			BC_DELETE(p_object);
		}
	}
}