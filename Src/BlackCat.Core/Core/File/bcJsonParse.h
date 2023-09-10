// [07/03/2017 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "3rdParty/RapidJSON/include/document.h"

namespace black_cat
{
	namespace core
	{
		using bc_native_json_document = rapidjson::Document;
		using bc_native_json_value = rapidjson::Document::ValueType;
	}
	
	namespace json_parse
	{
		// Define json_parse namespace

		template<typename T>
		void bc_load(const core::bc_native_json_value& p_jvalue, T& p_value)
		{
			BC_ASSERT(false);
		}

		template<typename T>
		void bc_write(core::bc_native_json_document& p_document, core::bc_native_json_value& p_jvalue, const T& p_value)
		{
			BC_ASSERT(false);
		}
	}
}