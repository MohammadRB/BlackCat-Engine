// [22/01/2022 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_icon_component::bc_icon_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
			: bci_actor_component(p_actor_id, p_id),
			m_size(s_default_icon_size),
			m_mode(bc_icon_mode::editor)
		{
		}

		bc_actor bc_icon_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_icon_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto* l_name = static_cast<core::bc_string*>(nullptr);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_icon_name, l_name);
			if(!l_name)
			{
				json_parse::bc_load(p_context.m_instance_parameters, constant::g_param_icon_name, l_name);
			}
			
			bcUINT32 l_size;
			const auto l_has_size = json_parse::bc_load(p_context.m_parameters, constant::g_param_icon_size, l_size);
			if (!l_has_size)
			{
				json_parse::bc_load(p_context.m_instance_parameters, constant::g_param_icon_size, l_size);
			}
			
			const auto* l_type = static_cast<core::bc_string*>(nullptr);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_icon_type, l_type);
			if (!l_type)
			{
				json_parse::bc_load(p_context.m_instance_parameters, constant::g_param_icon_type, l_type);
			}

			if(l_name)
			{
				m_name = *l_name;
			}
			if(l_size)
			{
				m_size = l_size;
			}
			if(l_type)
			{
				if(*l_type == "editor")
				{
					m_mode = bc_icon_mode::editor;
				}
			}
		}

		void bc_icon_component::set_icon(core::bc_string p_name) noexcept
		{
			set_icon(std::move(p_name), s_default_icon_size, bc_icon_mode::editor);
		}

		void bc_icon_component::set_icon(core::bc_string p_name, bcUINT16 p_size, bc_icon_mode p_mode) noexcept
		{
			m_name = std::move(p_name);
			m_size = p_size;
			m_mode = p_mode;
		}
	}
}
