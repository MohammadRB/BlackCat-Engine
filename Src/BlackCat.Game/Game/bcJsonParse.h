// [07/01/2022 MRB]

#pragma once

#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/Component/bcHumanRagdollComponent.h"
#include "Game/bcUtility.h"

namespace black_cat::json_parse
{
	inline bool bc_load(const core::bc_json_key_value& p_key_value, core::bc_string_view p_key, game::bc_human_ragdoll_transforms& p_value) noexcept
	{
		const auto l_value = p_key_value.find(p_key);
		if (l_value == std::cend(p_key_value))
		{
			return false;
		}

		auto* l_array = l_value->second.as<core::bc_vector<core::bc_any>>();
		if (!l_array || l_array->size() != p_value.size())
		{
			return false;
		}

		for(auto l_ite = 0U; l_ite < l_array->size(); ++l_ite)
		{
			auto* l_entry = (*l_array)[l_ite].as<core::bc_json_key_value>();
			if(!l_entry)
			{
				return false;
			}

			core::bc_vector3f l_position;
			core::bc_matrix3f l_rotation;

			bool l_values_read = false;

			l_values_read |= bc_load(*l_entry, "p", l_position);
			l_values_read |= bc_load(*l_entry, "r", l_rotation);

			if(!l_values_read)
			{
				return false;
			}

			p_value[l_ite] = physics::bc_transform(l_position, l_rotation);
		}

		return true;
	}

	inline void bc_write(core::bc_json_key_value& p_key_value, core::bc_string p_key, const game::bc_human_ragdoll_transforms& p_value) noexcept
	{
		core::bc_vector<core::bc_any> l_array(p_value.size());

		for (auto l_ite = 0U; l_ite < p_value.size(); l_ite++)
		{
			core::bc_json_key_value l_entry;

			auto l_position = p_value[l_ite].get_position();
			auto l_rotation = p_value[l_ite].get_matrix3();

			bc_write(l_entry, "p", l_position);
			bc_write(l_entry, "r", l_rotation);

			l_array[l_ite].set_value(std::move(l_entry));
		}

		p_key_value.add_or_update(std::move(p_key), core::bc_any(std::move(l_array)));
	}
}
