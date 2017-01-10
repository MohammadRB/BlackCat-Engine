// [06/16/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcParameterPack.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcHierarchyComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_hierarchy_component::bc_hierarchy_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index)
		{
		}

		bc_hierarchy_component::bc_hierarchy_component(bc_hierarchy_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other)),
			m_actors(std::move(p_other.m_actors))
		{
		}

		bc_hierarchy_component::~bc_hierarchy_component()
		{
			for(bc_actor& l_actor : m_actors)
			{
				l_actor.destroy();
			}
		}

		bc_hierarchy_component& bc_hierarchy_component::operator=(bc_hierarchy_component&& p_other) noexcept
		{
			bc_iactor_component::operator=(std::move(p_other));
			m_actors = std::move(p_other.m_actors);

			return *this;
		}

		void bc_hierarchy_component::add_actor(const bc_actor& p_actor)
		{
			m_actors.push_back(p_actor);
		}

		void bc_hierarchy_component::remove_actor(const bc_actor& p_actor)
		{
			auto l_actor = std::find_if(std::begin(m_actors), std::end(m_actors), [p_actor](const bc_actor& p_item)
			{
				return p_item == p_actor;
			});

			if(l_actor != std::end(m_actors))
			{
				m_actors.erase(l_actor);
			}
		}

		void bc_hierarchy_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			auto& l_child_entity_names = p_parameters.get_value_throw< core::bc_string >(core::g_param_child_actor);
			auto* l_entity_manager = core::bc_service_manager::get().get_service< bc_entity_manager >();

			core::bc_string::size_type l_last_pos = 0;
			core::bc_string::size_type l_pos = 0;
			core::bc_string l_child_entity_name;
			while ((l_pos = l_child_entity_names.find(':')) != std::string::npos)
			{
				l_child_entity_name = l_child_entity_names.substr(l_last_pos, l_pos);
				l_last_pos = l_pos;

				bc_actor l_child_actor = l_entity_manager->create_entity(l_child_entity_name.c_str());

				add_actor(l_child_actor);
			}
		}

		void bc_hierarchy_component::update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}
	}
}