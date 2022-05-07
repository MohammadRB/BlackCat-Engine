// [12/16/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_particle_emitter_component::bc_particle_emitter_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
			: bci_actor_component(p_actor_id, p_id),
			m_scene(nullptr)
		{
		}

		bc_actor bc_particle_emitter_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		bc_external_particle_emitter* bc_particle_emitter_component::get_emitter() noexcept
		{
			return m_emitter.get();
		}

		void bc_particle_emitter_component::add_emitter(const bcCHAR* p_emitter, 
			const core::bc_vector3f& p_position, 
			const core::bc_vector3f& p_dir,
			const core::bc_vector3f* p_color)
		{
			m_emitter = m_scene->get_particle_manager().add_emitter(p_emitter, p_position, p_dir, p_color);
		}

		void bc_particle_emitter_component::add_emitter(const bc_particle_builder& p_builder, 
			const core::bc_vector3f& p_position, 
			const core::bc_vector3f& p_dir,
			const core::bc_vector3f* p_color)
		{
			m_emitter = m_scene->get_particle_manager().add_emitter(p_builder, p_position, p_dir, p_color);
		}
		
		void bc_particle_emitter_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_scene = &p_context.m_scene;

			const core::bc_string* l_emitter_name_param = nullptr;
			core::bc_vector3f l_particle_color_param;

			json_parse::bc_load(p_context.m_parameters, constant::g_param_particle_emitter_name, l_emitter_name_param);
			const auto l_has_color = json_parse::bc_load(p_context.m_parameters, constant::g_param_particle_color, l_particle_color_param);
			
			if(l_emitter_name_param)
			{
				add_emitter
				(
					l_emitter_name_param->c_str(), 
					p_context.m_transform.get_translation(), 
					core::bc_vector3f::up(),
					l_has_color ? &l_particle_color_param : nullptr
				);
			}

			auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
			l_icon_component->set_icon(bc_icon_type::particle);
		}

		void bc_particle_emitter_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if(m_emitter)
			{
				if (const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
				{
					m_emitter->set_positions(l_world_transform_event->get_transform().get_translation());
				}
			}
		}
	}	
}