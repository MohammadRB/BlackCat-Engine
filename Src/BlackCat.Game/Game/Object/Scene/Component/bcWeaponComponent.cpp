// [03/27/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_weapon_component::bc_weapon_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			m_class(bc_weapon_class::rifle),
			m_main_hand_node_name(nullptr),
			m_second_hand_node_name(nullptr),
			m_rate_of_fire_seconds(0)
		{
		}

		bc_actor bc_weapon_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_weapon_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto& l_class_value = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_weapon_class);
			m_main_hand_node_name = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_weapon_main_hand_node).c_str();
			m_second_hand_node_name = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_weapon_second_hand_node).c_str();
			json_parse::bc_load(p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>(constant::g_param_weapon_main_hand_offset), m_main_hand_local_offset);
			json_parse::bc_load(p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>(constant::g_param_weapon_second_hand_offset), m_second_hand_local_offset);
			json_parse::bc_load(p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>(constant::g_param_weapon_local_up), m_local_up);
			json_parse::bc_load(p_context.m_parameters.get_value_throw<core::bc_vector<core::bc_any>>(constant::g_param_weapon_local_forward), m_local_forward);
			m_rate_of_fire_seconds = p_context.m_parameters.get_value_throw<bcFLOAT>(constant::g_param_weapon_rate_of_fire_seconds);
			
			if(l_class_value == "rifle")
			{
				m_class = bc_weapon_class::rifle;
			}
			else
			{
				throw bc_invalid_argument_exception("Weapon class value is invalid");
			}
		}
	}	
}