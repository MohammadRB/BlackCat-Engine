// [10/03/2017 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/bcException.h"

namespace black_cat::json_parse
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
				
	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, core::bc_vector2f& p_value) noexcept
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

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}

	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, core::bc_vector2i& p_value) noexcept
	{
		const auto l_value = p_key_value.find(p_key);
		if (l_value == std::cend(p_key_value))
		{
			return false;
		}

		auto* l_array = l_value->second.as<core::bc_vector<core::bc_any>>();
		if (!l_array || l_array->size() < 2)
		{
			return false;
		}

		auto* l_x = (*l_array)[0].as<bcINT32>();
		auto* l_y = (*l_array)[1].as<bcINT32>();
		if (!l_x || !l_y)
		{
			return false;
		}

		p_value.x = *l_x;
		p_value.y = *l_y;

		return true;
	}

	inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const core::bc_vector2i& p_value) noexcept
	{
		core::bc_vector<core::bc_any> l_array(2);
		l_array[0].set_value(p_value.x);
		l_array[1].set_value(p_value.y);

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}

	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, core::bc_vector3f& p_value) noexcept
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

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}

	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, core::bc_vector3i& p_value) noexcept
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

		auto* l_x = (*l_array)[0].as<bcINT32>();
		auto* l_y = (*l_array)[1].as<bcINT32>();
		auto* l_z = (*l_array)[2].as<bcINT32>();
		if (!l_x || !l_y || !l_z)
		{
			return false;
		}

		p_value.x = *l_x;
		p_value.y = *l_y;
		p_value.z = *l_z;

		return true;
	}

	inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const core::bc_vector3i& p_value) noexcept
	{
		core::bc_vector<core::bc_any> l_array(3);
		l_array[0].set_value(p_value.x);
		l_array[1].set_value(p_value.y);
		l_array[2].set_value(p_value.z);

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}

	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, core::bc_vector4f& p_value) noexcept
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

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}

	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, core::bc_vector4i& p_value) noexcept
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

		auto* l_x = (*l_array)[0].as<bcINT32>();
		auto* l_y = (*l_array)[1].as<bcINT32>();
		auto* l_z = (*l_array)[2].as<bcINT32>();
		auto* l_w = (*l_array)[3].as<bcINT32>();
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

	inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const core::bc_vector4i& p_value) noexcept
	{
		core::bc_vector<core::bc_any> l_array(4);
		l_array[0].set_value(p_value.x);
		l_array[1].set_value(p_value.y);
		l_array[2].set_value(p_value.z);
		l_array[3].set_value(p_value.w);

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}

	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, core::bc_matrix3f& p_value) noexcept
	{
		const auto l_value = p_key_value.find(p_key);
		if (l_value == std::cend(p_key_value))
		{
			return false;
		}

		auto* l_array = l_value->second.as<core::bc_vector<core::bc_any>>();
		if (!l_array || l_array->size() < 6)
		{
			return false;
		}

		auto* l_xx = (*l_array)[0].as<bcFLOAT>();
		auto* l_xy = (*l_array)[1].as<bcFLOAT>();
		auto* l_xz = (*l_array)[2].as<bcFLOAT>();
		auto* l_yx = (*l_array)[3].as<bcFLOAT>();
		auto* l_yy = (*l_array)[4].as<bcFLOAT>();
		auto* l_yz = (*l_array)[5].as<bcFLOAT>();
		if (!l_xx || !l_xy || !l_xz || !l_yx || !l_yy || !l_yz)
		{
			return false;
		}

		const core::bc_vector3f l_rotation_x(*l_xx, *l_xy, *l_xz);
		const core::bc_vector3f l_rotation_y(*l_yx, *l_yy, *l_yz);

		p_value.set_row(0, l_rotation_x);
		p_value.set_row(1, l_rotation_y);
		p_value.set_row(2, core::bc_vector3f::cross(l_rotation_x, l_rotation_y));

		return true;
	}

	inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const core::bc_matrix3f& p_value) noexcept
	{
		const auto l_x = p_value.get_row(0);
		const auto l_y = p_value.get_row(1);

		core::bc_vector<core::bc_any> l_array(6);
		l_array[0].set_value(l_x.x);
		l_array[1].set_value(l_x.y);
		l_array[2].set_value(l_x.z);
		l_array[3].set_value(l_y.x);
		l_array[4].set_value(l_y.y);
		l_array[5].set_value(l_y.z);

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}

	template<typename T>
	void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const T& p_value) noexcept
	{
		p_key_value.add_or_update(std::move(p_key), core::bc_any(p_value));
	}

	template<typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
	bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, T& p_value) noexcept
	{
		const auto l_value = p_key_value.find(p_key);
		if (l_value == std::cend(p_key_value))
		{
			return false;
		}

		auto* l_concrete_value = l_value->second.as<std::decay_t<T>>();
		if (!l_concrete_value)
		{
			return false;
		}

		p_value = *l_concrete_value;
		return true;
	}

	template<typename T>
	bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, const T*& p_value) noexcept
	{
		const auto l_value = p_key_value.find(p_key);
		if (l_value == std::cend(p_key_value))
		{
			return false;
		}

		auto* l_concrete_value = l_value->second.as<std::decay_t<T>>();
		if (!l_concrete_value)
		{
			return false;
		}

		p_value = l_concrete_value;
		return true;
	}

	template<typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
	void bc_load_throw(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, T& p_value) noexcept
	{
		const auto l_found = bc_load(p_key_value, p_key, p_value);
		if (!l_found)
		{
			throw bc_key_not_found_exception("json key was not found in json key value");
		}
	}

	template<typename T>
	void bc_load_throw(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, const T*& p_value) noexcept
	{
		const auto l_found = bc_load(p_key_value, p_key, p_value);
		if (!l_found)
		{
			throw bc_key_not_found_exception("json key was not found in json key value");
		}
	}
}
