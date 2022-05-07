// [06/16/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Utility/bcParameterPack.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcHierarchyComponent.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_hierarchy_component::bc_hierarchy_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
			: bci_actor_component(p_actor_id, p_id)
		{
		}

		bc_hierarchy_component::bc_hierarchy_component(bc_hierarchy_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
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
			bci_actor_component::operator=(std::move(p_other));
			m_actors = std::move(p_other.m_actors);

			return *this;
		}

		bc_actor bc_hierarchy_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		const core::bc_vector_movable< bc_actor >& bc_hierarchy_component::get_actors() const
		{
			return m_actors;
		}

		void bc_hierarchy_component::add_actor(const bc_actor& p_actor)
		{
			m_actors.push_back(p_actor);
		}

		void bc_hierarchy_component::remove_actor(const bc_actor& p_actor)
		{
			const auto l_actor = std::find_if(std::begin(m_actors), std::end(m_actors), [p_actor](const bc_actor& p_item)
			{
				return p_item == p_actor;
			});

			if(l_actor != std::end(m_actors))
			{
				m_actors.erase(l_actor);
			}
		}

		void bc_hierarchy_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto* l_child_entity_names = static_cast<core::bc_string*>(nullptr);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_child_actor, l_child_entity_names);
			
			core::bc_string::size_type l_last_pos = 0;
			core::bc_string::size_type l_pos = 0;
			core::bc_string l_child_entity_name;
			while ((l_pos = l_child_entity_names->find(':')) != std::string::npos)
			{
				l_child_entity_name = l_child_entity_names->substr(l_last_pos, l_pos);
				l_last_pos = l_pos;

				bc_actor l_child_actor = p_context.m_scene.create_actor(l_child_entity_name.c_str(), core::bc_matrix4f::identity());

				add_actor(l_child_actor);
			}
		}
	}
}