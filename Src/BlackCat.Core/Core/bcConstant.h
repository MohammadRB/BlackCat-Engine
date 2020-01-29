// [05/31/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace constant
	{
		using bc_render_pass_variable_t = const bcCHAR*;

#define BC_CONST(p_type, p_name, p_value)		constexpr p_type g_##p_name = p_value;

#define BC_PARAMETER(p_name, p_value)			constexpr const bcCHAR* g_param_##p_name = p_value

#define BC_RENDER_PASS_VARIABLE(name)			constexpr constant::bc_render_pass_variable_t g_rpass_##name = #name

#define BC_SERVICE_NAME(p_name)					"srv_" ## #p_name

#define BC_CONTENT_NAME(p_name)					"cnt_" ## #p_name

#define BC_COMPONENT_NAME(p_name)				"cmp_" ## #p_name

#define BC_EVENT_NAME(p_name)					"evt_" ## #p_name

#define BC_RENDER_PASS_NAME(p_name)				"rpass_" ## #p_name

#define BC_SERVICE(p_name) \
	public: \
	static constexpr const bcCHAR* service_name() \
	{ \
		return BC_SERVICE_NAME(p_name); \
	} \
	static constexpr bcUINT32 service_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_SERVICE_NAME(p_name)); \
	} \

#define BC_CONTENT(p_name) \
	public: \
	static constexpr const bcCHAR* content_name() \
	{ \
		return BC_CONTENT_NAME(p_name); \
	} \
	static constexpr bcUINT32 content_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_CONTENT_NAME(p_name)); \
	} \

#define BC_COMPONENT(p_name) \
	public: \
	static constexpr bool component_is_abstract() \
	{ \
		return false; \
	} \
	static constexpr const bcCHAR* component_name() \
	{ \
		return BC_COMPONENT_NAME(p_name); \
	} \
	static constexpr bcUINT32 component_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_COMPONENT_NAME(p_name)); \
	} \

#define BC_ABSTRACT_COMPONENT(p_name) \
	public: \
	static constexpr bool component_is_abstract() \
	{ \
		return true; \
	} \
	static constexpr const bcCHAR* component_name() \
	{ \
		return BC_COMPONENT_NAME(p_name); \
	} \
	static constexpr bcUINT32 component_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_COMPONENT_NAME(p_name)); \
	} \

#define BC_EVENT(p_name) \
	public: \
	static constexpr const bcCHAR* event_name() \
	{ \
		return BC_EVENT_NAME(p_name); \
	} \
	static constexpr bcUINT32 event_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_EVENT_NAME(p_name)); \
	} \

#define BC_RENDER_PASS(p_name) \
	public: \
	static constexpr const bcCHAR* render_pass_name() \
	{ \
		return BC_RENDER_PASS_NAME(p_name); \
	} \
	static constexpr bcUINT32 render_pass_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_RENDER_PASS_NAME(p_name)); \
	}
	}
}