// [03/10/2017 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/bcException.h"

namespace black_cat
{
	namespace json_parse
	{
		inline void bc_load(const core::bc_json_value_object& p_jvalue, core::bc_vector2f& p_value)
		{
			if(!p_jvalue.IsArray())
			{
				throw bc_io_exception("Bad json format");
			}

			const auto l_jarray = p_jvalue.GetArray();

			p_value.x = l_jarray.Size() > 0 ? l_jarray[0].GetFloat() : 0;
			p_value.y = l_jarray.Size() > 1 ? l_jarray[1].GetFloat() : 0;
		}

		inline void bc_write(core::bc_json_document_object& p_document, core::bc_json_value_object& p_jvalue, const core::bc_vector2f& p_value)
		{
			p_jvalue.SetArray();

			p_jvalue.PushBack(core::bc_json_value_object(p_value.x), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.y), p_document.GetAllocator());
		}

		inline void bc_load(const core::bc_json_value_object& p_jvalue, core::bc_vector2i& p_value)
		{
			if (!p_jvalue.IsArray())
			{
				throw bc_io_exception("Bad json format");
			}

			const auto l_jarray = p_jvalue.GetArray();

			p_value.x = l_jarray.Size() > 0 ? l_jarray[0].GetInt() : 0;
			p_value.y = l_jarray.Size() > 1 ? l_jarray[1].GetInt() : 0;
		}

		inline void bc_write(core::bc_json_document_object& p_document, core::bc_json_value_object& p_jvalue, const core::bc_vector2i& p_value)
		{
			p_jvalue.SetArray();

			p_jvalue.PushBack(core::bc_json_value_object(p_value.x), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.y), p_document.GetAllocator());
		}

		inline void bc_load(const core::bc_json_value_object& p_jvalue, core::bc_vector3f& p_value)
		{
			if (!p_jvalue.IsArray())
			{
				throw bc_io_exception("Bad json format");
			}

			const auto l_jarray = p_jvalue.GetArray();

			p_value.x = l_jarray.Size() > 0 ? l_jarray[0].GetFloat() : 0;
			p_value.y = l_jarray.Size() > 1 ? l_jarray[1].GetFloat() : 0;
			p_value.z = l_jarray.Size() > 2 ? l_jarray[2].GetFloat() : 0;
		}

		inline void bc_write(core::bc_json_document_object& p_document, core::bc_json_value_object& p_jvalue, const core::bc_vector3f& p_value)
		{
			p_jvalue.SetArray();

			p_jvalue.PushBack(core::bc_json_value_object(p_value.x), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.y), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.z), p_document.GetAllocator());
		}

		inline void bc_load(const core::bc_json_value_object& p_jvalue, core::bc_vector3i& p_value)
		{
			if (!p_jvalue.IsArray())
			{
				throw bc_io_exception("Bad json format");
			}

			const auto l_jarray = p_jvalue.GetArray();

			p_value.x = l_jarray.Size() > 0 ? l_jarray[0].GetInt() : 0;
			p_value.y = l_jarray.Size() > 1 ? l_jarray[1].GetInt() : 0;
			p_value.z = l_jarray.Size() > 3 ? l_jarray[3].GetInt() : 0;
		}

		inline void bc_write(core::bc_json_document_object& p_document, core::bc_json_value_object& p_jvalue, const core::bc_vector3i& p_value)
		{
			p_jvalue.SetArray();

			p_jvalue.PushBack(core::bc_json_value_object(p_value.x), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.y), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.z), p_document.GetAllocator());
		}

		inline void bc_load(const core::bc_json_value_object& p_jvalue, core::bc_vector4f& p_value)
		{
			if (!p_jvalue.IsArray())
			{
				throw bc_io_exception("Bad json format");
			}

			const auto l_jarray = p_jvalue.GetArray();

			p_value.x = l_jarray.Size() > 0 ? l_jarray[0].GetFloat() : 0;
			p_value.y = l_jarray.Size() > 1 ? l_jarray[1].GetFloat() : 0;
			p_value.z = l_jarray.Size() > 2 ? l_jarray[2].GetFloat() : 0;
			p_value.w = l_jarray.Size() > 3 ? l_jarray[3].GetFloat() : 0;
		}

		inline void bc_write(core::bc_json_document_object& p_document, core::bc_json_value_object& p_jvalue, const core::bc_vector4f& p_value)
		{
			p_jvalue.SetArray();

			p_jvalue.PushBack(core::bc_json_value_object(p_value.x), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.y), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.z), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.w), p_document.GetAllocator());
		}

		inline void bc_load(const core::bc_json_value_object& p_jvalue, core::bc_vector4i& p_value)
		{
			if (!p_jvalue.IsArray())
			{
				throw bc_io_exception("Bad json format");
			}

			const auto l_jarray = p_jvalue.GetArray();

			p_value.x = l_jarray.Size() > 0 ? l_jarray[0].GetInt() : 0;
			p_value.y = l_jarray.Size() > 1 ? l_jarray[1].GetInt() : 0;
			p_value.z = l_jarray.Size() > 2 ? l_jarray[2].GetInt() : 0;
			p_value.w = l_jarray.Size() > 3 ? l_jarray[3].GetInt() : 0;
		}

		inline void bc_write(core::bc_json_document_object& p_document, core::bc_json_value_object& p_jvalue, const core::bc_vector4i& p_value)
		{
			p_jvalue.SetArray();

			p_jvalue.PushBack(core::bc_json_value_object(p_value.x), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.y), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.z), p_document.GetAllocator());
			p_jvalue.PushBack(core::bc_json_value_object(p_value.w), p_document.GetAllocator());
		}

		inline void bc_load(const core::bc_vector<core::bc_any>& p_jvalue, core::bc_vector2f& p_value)
		{
			if (p_jvalue.size() != 2)
			{
				throw bc_invalid_operation_exception("Input array must contains 2 elements");
			}

			p_value = { p_jvalue[0].as_throw<bcFLOAT>(), p_jvalue[1].as_throw<bcFLOAT>() };
		}
		
		inline void bc_load(const core::bc_vector<core::bc_any>& p_jvalue, core::bc_vector3f& p_value)
		{
			if(p_jvalue.size() != 3)
			{
				throw bc_invalid_operation_exception("Input array must contains 3 elements");
			}

			p_value = { p_jvalue[0].as_throw<bcFLOAT>(), p_jvalue[1].as_throw<bcFLOAT>() , p_jvalue[2].as_throw<bcFLOAT>() };
		}

		inline void bc_load(const core::bc_vector<core::bc_any>& p_jvalue, core::bc_vector4f& p_value)
		{
			if (p_jvalue.size() != 4)
			{
				throw bc_invalid_operation_exception("Input array must contains 4 elements");
			}

			p_value = { p_jvalue[0].as_throw<bcFLOAT>(), p_jvalue[1].as_throw<bcFLOAT>() , p_jvalue[2].as_throw<bcFLOAT>(), p_jvalue[3].as_throw<bcFLOAT>() };
		}
	}
}