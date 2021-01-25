// [12/31/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Animation/bcAnimationManager.h"
#include "Game/bcConstant.h"
#include "Game/bcUtility.h"

namespace black_cat
{
	namespace game
	{
		bc_skinned_mesh_component::bc_skinned_mesh_component(bc_actor_component_index p_index)
			: bc_mesh_component(p_index),
			m_animation_played(false)
		{
		}

		bc_skinned_mesh_component::bc_skinned_mesh_component(bc_skinned_mesh_component&& p_other) noexcept = default;

		bc_skinned_mesh_component::~bc_skinned_mesh_component() = default;

		bc_skinned_mesh_component& bc_skinned_mesh_component::operator=(bc_skinned_mesh_component&& p_other) noexcept = default;

		bc_actor bc_skinned_mesh_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_skinned_mesh_component::add_animation_job(bci_animation_job& p_animation_job) noexcept
		{
			core::bc_get_service<bc_game_system>()->get_render_system().get_animation_manager().schedule_job(p_animation_job);
			m_animation_played = true;
		}

		void bc_skinned_mesh_component::initialize(bc_actor_component_initialize_context& p_context)
		{
			bc_mesh_component::initialize(p_context);
			
			const auto& l_animation_names = p_context.m_parameters.get_value_throw< core::bc_vector< core::bc_any > >(constant::g_param_animations);
			
			m_model_transforms = bc_sub_mesh_mat4_transform(*get_mesh().get_root_node());
			m_collider_model_transforms = bc_sub_mesh_px_transform(*get_mesh().get_root_node());
			m_animations.reserve(l_animation_names.size());

			for(auto& l_animation_name : l_animation_names)
			{
				auto l_skinned_animation = p_context.m_stream_manager.find_content_throw< bc_skinned_animation >(l_animation_name.as_throw<core::bc_string>()->c_str());
				m_animations.push_back(std::move(l_skinned_animation));
			}

			m_animations.shrink_to_fit();
		}

		void bc_skinned_mesh_component::update(bc_actor_component_update_content& p_context)
		{
		}

		void bc_skinned_mesh_component::handle_event(bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_actor_event_world_transform>(p_context.m_event);
			if (l_world_transform_event)
			{
				_set_world_transform(p_context.m_actor, l_world_transform_event->get_transform());
			}
		}
		
		void bc_skinned_mesh_component::render(bc_render_state_buffer& p_buffer) const
		{
			const auto& l_mesh = get_mesh();
			const bc_mesh_node* l_root_pointer = l_mesh.get_root_node();
			render_skinned_mesh(p_buffer, l_mesh, get_world_transforms(), &l_root_pointer, &l_root_pointer + 1);
		}

		void bc_skinned_mesh_component::debug_draw(bc_actor_component_debug_draw_context& p_context)
		{
			const auto& l_mesh = get_mesh();
			const auto* l_mediate_component = p_context.m_actor.get_component< bc_mediate_component >();
			const auto l_bound_box_z_length = l_mediate_component->get_bound_box().get_half_extends().z;
			auto l_world_transform = l_mediate_component->get_world_transform();
			l_world_transform.set_translation(l_world_transform.get_translation() + core::bc_vector3f(0,0, l_bound_box_z_length * 1.1f));
			
			p_context.m_shape_drawer.draw_wired_skeleton
			(
				l_mesh,
				l_world_transform,
				m_model_transforms
			);
		}

		void bc_skinned_mesh_component::_set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform)
		{
			// mesh is in bind pose
			if(!m_animation_played)
			{
				const auto& l_mesh = get_mesh();
				auto& l_world_transforms = get_world_transforms();

				physics::bc_bound_box l_bound_box;
				l_mesh.calculate_absolute_transforms(p_transform, l_world_transforms, l_bound_box);
				
				bcINT32 l_dummy;
				l_mesh.iterate_over_nodes(l_dummy, [this, &l_mesh, &p_transform, &l_world_transforms](const bc_mesh_node& p_node, bcINT32)
				{
					// Reset absolute transforms so skinned mesh will be rendered correctly in T pose
					l_world_transforms.set_node_transform(p_node, core::bc_matrix4f::scale_matrix(l_mesh.get_mesh_scale()) * p_transform);

					return 0;
				});

				p_actor.add_event(bc_actor_event_bound_box_changed(l_bound_box));
			}
		}
	}
}