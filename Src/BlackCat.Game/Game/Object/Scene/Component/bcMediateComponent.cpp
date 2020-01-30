// [12/22/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/Object/Scene/Component/Event/bcActorEventAddedToScene.h"

namespace black_cat
{
	namespace game
	{
		bc_mediate_component::bc_mediate_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index),
			m_entity_name(nullptr),
			m_scene(nullptr),
			m_bound_box_changed(false),
			m_bound_box(core::bc_vector3f(0), core::bc_vector3f(1)),
			m_prev_bound_box(core::bc_vector3f(0), core::bc_vector3f(1))
		{
		}

		bc_actor bc_mediate_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_mediate_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
		}

		void bc_mediate_component::write_instance(bc_actor& p_actor, core::bc_json_key_value& p_parameters)
		{
			p_parameters.add(std::make_pair(core::bc_string(s_entity_name_json_key), core::bc_any(m_entity_name)));
			p_parameters.add(std::make_pair(core::bc_string(s_position_json_key), core::bc_any(get_world_position())));
		}

		void bc_mediate_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			auto* l_bound_box_event = core::bc_event::event_as< bc_actor_event_bound_box_changed >(p_event);
			if(l_bound_box_event)
			{
				m_prev_bound_box = m_bound_box;
				m_bound_box = l_bound_box_event->get_bound_box();
				m_bound_box_changed = true;
				return;
			}

			auto* l_added_to_scene_event = core::bc_event::event_as< bc_actor_event_added_to_scene >(p_event);
			if(l_added_to_scene_event)
			{
				m_scene = &l_added_to_scene_event->get_scene();
				return;
			}
		}
		
		void bc_mediate_component::update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			if(m_scene && m_bound_box_changed)
			{
				m_scene->update_actor(p_actor);
				m_bound_box_changed = false;
			}
		}
	}
}