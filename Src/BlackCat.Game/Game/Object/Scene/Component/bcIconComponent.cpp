// [01/22/2022 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_icon_component::bc_icon_component(bc_actor_id p_actor_index, bc_actor_component_id p_index)
			: bci_actor_component(p_actor_index, p_index),
			m_size(32),
			m_type(bc_icon_type::editor)
		{
		}

		bc_actor bc_icon_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_icon_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto* l_name = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_icon_name);
			if(l_name)
			{
				m_name = *l_name;
			}
			else
			{
				m_name = p_context.m_instance_parameters.get_value_throw<core::bc_string>(constant::g_param_icon_name);
			}

			auto* l_size = p_context.m_parameters.get_value<bcUINT32>(constant::g_param_icon_size);
			if (!l_size)
			{
				l_size = p_context.m_instance_parameters.get_value<bcUINT32>(constant::g_param_icon_size);
			}

			const auto* l_type = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_icon_type);
			if(!l_type)
			{
				l_type = p_context.m_instance_parameters.get_value<core::bc_string>(constant::g_param_icon_type);
			}

			if(l_size)
			{
				m_size = *l_size;
			}

			if(l_type)
			{
				if(*l_type == "editor")
				{
					m_type = bc_icon_type::editor;
				}
			}
		}
	}
}
