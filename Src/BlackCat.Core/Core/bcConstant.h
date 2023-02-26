// [05/31/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace constant
	{
#define BC_STR_CONCAT(p_str1, p_str2)			p_str1 p_str2

#define BC_CONST(p_type, p_name, p_value)		constexpr p_type g_##p_name = p_value;

#define BC_PARAMETER(p_name, p_value)			constexpr const bcCHAR* g_param_##p_name = p_value

#define BC_RENDER_PASS_VARIABLE(p_name)			constexpr const bcCHAR* g_rpass_##p_name = #p_name

#define BC_CONFIG_VARIABLE(p_name)				constexpr const bcCHAR* g_cng_##p_name = #p_name

#define BC_SERVICE_NAME(p_name)					BC_STR_CONCAT("srv_", #p_name)

#define BC_CONTENT_NAME(p_name)					BC_STR_CONCAT("cnt_", #p_name)

#define BC_COMPONENT_NAME(p_name)				BC_STR_CONCAT("cmp_", #p_name)
				
#define BC_EVENT_NAME(p_name)					BC_STR_CONCAT("evt_", #p_name)

#define BC_QUERY_NAME(p_name)					BC_STR_CONCAT("qur_", #p_name)

#define BC_NETWORK_MESSAGE_NAME(p_name)			BC_STR_CONCAT("msg_", #p_name)
		
#define BC_RENDER_PASS_NAME(p_name)				BC_STR_CONCAT("rps_", #p_name)
		
#define BC_SERVICE(p_name) \
	public: \
	static constexpr const bcCHAR* service_name() \
	{ \
		return BC_SERVICE_NAME(p_name); \
	} \
	static constexpr core::bc_string_cmp_hash service_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_SERVICE_NAME(p_name)); \
	} \

#define BC_CONTENT(p_name) \
	public: \
	static constexpr const bcCHAR* content_name() \
	{ \
		return BC_CONTENT_NAME(p_name); \
	} \
	static constexpr core::bc_string_cmp_hash content_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_CONTENT_NAME(p_name)); \
	} \

#define BC_COMPONENT_DEFINITION(p_name, p_is_abstract, p_require_event, p_require_update) \
	public: \
	static constexpr bool component_is_abstract() \
	{ \
		return p_is_abstract; \
	} \
	static constexpr bool component_require_event() \
	{ \
		return p_require_event; \
	} \
	static constexpr bool component_require_update() \
	{ \
		return p_require_update; \
	} \
	static constexpr const bcCHAR* component_name() \
	{ \
		return BC_COMPONENT_NAME(p_name); \
	} \
	static constexpr core::bc_string_cmp_hash component_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_COMPONENT_NAME(p_name)); \
	} \
	
#define BC_COMPONENT(p_name, p_require_event, p_require_update) BC_COMPONENT_DEFINITION(p_name, false, p_require_event, p_require_update)

#define BC_ABSTRACT_COMPONENT(p_name) BC_COMPONENT_DEFINITION(p_name, true, false, false)

#define BC_EVENT(p_name) \
	public: \
	static constexpr const bcCHAR* message_name() \
	{ \
		return BC_EVENT_NAME(p_name); \
	} \
	static constexpr core::bc_string_cmp_hash message_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_EVENT_NAME(p_name)); \
	} \

#define BC_QUERY_DEFINITION(p_name, p_is_shared) \
	public: \
	static constexpr const bcCHAR* message_name() \
	{ \
		return BC_QUERY_NAME(p_name); \
	} \
	static constexpr core::bc_string_cmp_hash message_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_QUERY_NAME(p_name)); \
	} \
	static constexpr bool is_shared() \
	{ \
		return p_is_shared; \
	} \

#define BC_QUERY(p_name) BC_QUERY_DEFINITION(p_name, false)
		
#define BC_SHARED_QUERY(p_name) BC_QUERY_DEFINITION(p_name, true)

#define BC_NETWORK_MESSAGE(p_name) \
	public: \
	static constexpr const bcCHAR* message_name() \
	{ \
		return BC_NETWORK_MESSAGE_NAME(p_name); \
	} \
	static constexpr core::bc_string_cmp_hash message_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_NETWORK_MESSAGE_NAME(p_name)); \
	}
		
#define BC_RENDER_PASS(p_name) \
	public: \
	static constexpr const bcCHAR* render_pass_name() \
	{ \
		return BC_RENDER_PASS_NAME(p_name); \
	} \
	static constexpr core::bc_string_cmp_hash render_pass_hash() \
	{ \
		return BC_COMPILE_TIME_STRING_HASH(BC_RENDER_PASS_NAME(p_name)); \
	}
	
	}
}