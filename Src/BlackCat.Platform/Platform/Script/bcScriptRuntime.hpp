 // [16/10/2016 MRB]

/*
 * This file is created duo to circular dependency between 
 * bcScriptRuntime.h -> bcScriptExternalObject.h -> bcScriptPrototype.h -> bcScriptContext.h -> bcScriptRuntime.h
 */

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Platform/Script/bcScriptRuntime.h"
#include "PlatformImp/Script/bcScriptExternalObject.h"

namespace black_cat
{
	namespace platform
	{
		template<bc_platform TPlatform>
		template<typename T, typename ...TA>
		bc_script_external_object<T>* bc_platform_script_runtime<TPlatform>::create_native(TA&&... p_args)
		{
			return BC_NEW(bc_script_external_object<T>(T(std::forward<TA>(p_args)...)), core::bc_alloc_type::unknown);
		}

		template<bc_platform TPlatform>
		template<typename T>
		void bc_platform_script_runtime<TPlatform>::destroy_native(bc_script_external_object<T>* p_object)
		{
			BC_DELETE(p_object);
		}

		template<bc_platform TPlatform>
		template<typename T>
		typename bc_script_external_object<T>::meta_data* bc_platform_script_runtime<TPlatform>::_create_external_object_meta_data()
		{
			m_external_object_meta_data.push_back(core::bc_any(typename bc_script_external_object<T>::meta_data()));

			auto* l_meta_data = m_external_object_meta_data.back().as<typename bc_script_external_object<T>::meta_data>();

			return l_meta_data;
		}
	}
}