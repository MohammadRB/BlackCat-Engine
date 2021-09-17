// [03/10/2017 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
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

		inline bool bc_load(const core::bc_json_key_value& p_key_value, const bcCHAR* p_key, core::bc_vector2f& p_value) noexcept
		{
			const auto l_value = p_key_value.find(p_key);
			if(l_value == std::cend(p_key_value))
			{
				return false;
			}

			auto* l_array = l_value->second.as<core::bc_vector<core::bc_any>>();
			if (!l_array || l_array->size() < 2)
			{
				return false;
			}

			auto* l_x = (*l_array)[0].as<bcFLOAT>();
			auto* l_y = (*l_array)[1].as<bcFLOAT>();
			if (!l_x || !l_y)
			{
				return false;
			}

			p_value.x = *l_x;
			p_value.y = *l_y;

			return true;
		}

		inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const core::bc_vector2f& p_value) noexcept
		{
			core::bc_vector<core::bc_any> l_array(2);
			l_array[0].set_value(p_value.x);
			l_array[1].set_value(p_value.y);

			p_key_value.add(std::move(p_key), core::bc_any(std::move(l_array)));
		}

		inline bool bc_load(const core::bc_json_key_value& p_key_value, const bcCHAR* p_key, core::bc_vector3f& p_value) noexcept
		{
			const auto l_value = p_key_value.find(p_key);
			if (l_value == std::cend(p_key_value))
			{
				return false;
			}

			auto* l_array = l_value->second.as<core::bc_vector<core::bc_any>>();
			if (!l_array || l_array->size() < 3)
			{
				return false;
			}

			auto* l_x = (*l_array)[0].as<bcFLOAT>();
			auto* l_y = (*l_array)[1].as<bcFLOAT>();
			auto* l_z = (*l_array)[2].as<bcFLOAT>();
			if (!l_x || !l_y || !l_z)
			{
				return false;
			}

			p_value.x = *l_x;
			p_value.y = *l_y;
			p_value.z = *l_z;

			return true;
		}

		inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const core::bc_vector3f& p_value) noexcept
		{
			core::bc_vector<core::bc_any> l_array(3);
			l_array[0].set_value(p_value.x);
			l_array[1].set_value(p_value.y);
			l_array[2].set_value(p_value.z);

			p_key_value.add(std::move(p_key), core::bc_any(std::move(l_array)));
		}

		inline bool bc_load(const core::bc_json_key_value& p_key_value, const bcCHAR* p_key, core::bc_vector4f& p_value) noexcept
		{
			const auto l_value = p_key_value.find(p_key);
			if (l_value == std::cend(p_key_value))
			{
				return false;
			}

			auto* l_array = l_value->second.as<core::bc_vector<core::bc_any>>();
			if (!l_array || l_array->size() < 4)
			{
				return false;
			}

			auto* l_x = (*l_array)[0].as<bcFLOAT>();
			auto* l_y = (*l_array)[1].as<bcFLOAT>();
			auto* l_z = (*l_array)[2].as<bcFLOAT>();
			auto* l_w = (*l_array)[3].as<bcFLOAT>();
			if (!l_x || !l_y || !l_z || !l_w)
			{
				return false;
			}

			p_value.x = *l_x;
			p_value.y = *l_y;
			p_value.z = *l_z;
			p_value.w = *l_w;

			return true;
		}

		inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const core::bc_vector4f& p_value) noexcept
		{
			core::bc_vector<core::bc_any> l_array(4);
			l_array[0].set_value(p_value.x);
			l_array[1].set_value(p_value.y);
			l_array[2].set_value(p_value.z);
			l_array[3].set_value(p_value.w);

			p_key_value.add(std::move(p_key), core::bc_any(std::move(l_array)));
		}
	}
}