// [09/09/2019 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcSimpleMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_simple_mesh_component::bc_simple_mesh_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			bc_mesh_component()
		{
		}

		bc_simple_mesh_component::bc_simple_mesh_component(bc_simple_mesh_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
			bc_mesh_component(std::move(p_other))
		{
		}

		bc_simple_mesh_component::~bc_simple_mesh_component()
		{
		}

		bc_simple_mesh_component& bc_simple_mesh_component::operator=(bc_simple_mesh_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));
			bc_mesh_component::operator=(std::move(p_other));
			return *this;
		}

		bc_actor bc_simple_mesh_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_simple_mesh_component::render(const bc_actor_component_render_context& p_context) const
		{
			const auto l_mesh_lod = get_mesh().get_mesh_level_of_detail();
			const auto l_lod = l_mesh_lod.get_lod_culling
			(
				p_context.m_camera.m_main_camera.get_position(),
				p_context.m_camera.m_render_camera.get_position(),
				get_world_position(),
				get_lod_factor()
			);
			if (!l_lod.second)
			{
				return;
			}
			
			if(!m_render_group.is_set())
			{
				auto* l_rigid_body_component = get_actor().get_component<bc_rigid_body_component>();
				if(l_rigid_body_component)
				{
					const auto l_px_actor_type = l_rigid_body_component->get_body().get_type();
					if (l_px_actor_type == physics::bc_actor_type::rigid_static)
					{
						m_render_group.reset(bc_render_group::static_mesh);
					}
					else
					{
						m_render_group.reset(bc_render_group::dynamic_mesh);
					}
				}
				else
				{
					m_render_group.reset(bc_render_group::static_mesh);
				}
			}

			const auto& l_render_states = get_render_states();
			const auto& l_world_transforms = get_world_transforms();
			bc_mesh_utility::render_mesh(p_context.m_buffer, l_render_states, l_world_transforms, l_lod.first, *m_render_group.get());
		}
		
		void bc_simple_mesh_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				bc_mesh_component::set_world_transform(p_context.m_actor, l_world_transform_event->get_transform());
				return;
			}

			const auto* l_bound_box_event = core::bci_message::as<bc_bound_box_changed_actor_event>(p_context.m_event);
			if(l_bound_box_event)
			{
				bc_mesh_component::update_lod_factor(l_bound_box_event->get_bound_box());
			}
		}
	}
}