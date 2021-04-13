// [12/31/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Animation/bcAnimationManager.h"
#include "Game/bcConstant.h"
#include "Game/bcUtility.h"

namespace black_cat
{
	namespace game
	{
		bc_skinned_mesh_component::bc_skinned_mesh_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			bc_mesh_component(),
			m_animation_manager(nullptr),
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

		bc_skeleton_animation* bc_skinned_mesh_component::find_animation(const bcCHAR* p_name) const noexcept
		{
			if(!p_name)
			{
				return nullptr;
			}
			
			const auto l_ite = std::find_if
			(
				std::begin(m_all_animations),
				std::end(m_all_animations),
				[=](bc_skeleton_animation* p_animation)
				{
					return std::strstr(p_animation->get_name(), p_name) != nullptr;
				}
			);

			if (l_ite != std::end(m_all_animations))
			{
				return *l_ite;
			}

			return nullptr;
		}

		void bc_skinned_mesh_component::add_animation_job(bci_animation_job* p_animation_job) noexcept
		{
			if(p_animation_job)
			{
				m_animation_manager->schedule_job(*p_animation_job);
				m_animation_played = true;
			}
			else
			{
				m_animation_played = false;
			}
		}

		void bc_skinned_mesh_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			bc_mesh_component::initialize(p_context);
			
			const auto& l_animation_names = p_context.m_parameters.get_value_throw< core::bc_vector< core::bc_any > >(constant::g_param_animations);

			m_animation_manager = &p_context.m_game_system.get_render_system().get_animation_manager();
			m_model_transforms = bc_sub_mesh_mat4_transform(*get_mesh().get_root_node());
			m_collider_model_transforms = bc_sub_mesh_px_transform(*get_mesh().get_root_node());
			m_animations.reserve(l_animation_names.size());
			m_all_animations.reserve(l_animation_names.size());
			bool l_has_invalid_skeletons = false;
			
			for(auto& l_animation_name : l_animation_names)
			{
				bc_skinned_animation_ptr l_skinned_animation = p_context.m_stream_manager.find_content_throw< bc_skinned_animation >(l_animation_name.as_throw<core::bc_string>().c_str());

				if(!m_animations.empty())
				{
					if(l_skinned_animation->get_skeleton().get_num_joints() != (*m_animations.rbegin())->get_skeleton().get_num_joints())
					{
						l_has_invalid_skeletons = true;
					}
				}
				
				for (bc_skeleton_animation& l_animation : l_skinned_animation->get_animations())
				{
					m_all_animations.push_back(&l_animation);
				}
				m_animations.push_back(std::move(l_skinned_animation));
			}

			if(l_has_invalid_skeletons)
			{
				const auto* l_entity_name = p_context.m_actor.get_component<bc_mediate_component>()->get_entity_name();
				const auto l_msg =  core::bc_to_estring_frame(l_entity_name) + bcL(": The number of bones in animation skeletons are different");
				core::bc_get_service<core::bc_logger>()->log_debug(l_msg.c_str());
			}
			
			m_animations.shrink_to_fit();
		}

		void bc_skinned_mesh_component::update(const bc_actor_component_update_content& p_context)
		{
		}

		void bc_skinned_mesh_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as< bc_world_transform_actor_event >(p_context.m_event);
			if (l_world_transform_event)
			{
				_set_world_transform(p_context.m_actor, l_world_transform_event->get_transform());
				return;
			}

			const auto* l_bound_box_event = core::bci_message::as< bc_bound_box_changed_actor_event >(p_context.m_event);
			if (l_bound_box_event)
			{
				bc_mesh_component::update_lod_factor(l_bound_box_event->get_bound_box());
			}
		}
		
		void bc_skinned_mesh_component::render(const bc_actor_component_render_context& p_context) const
		{
			const auto l_mesh_lod = get_mesh().get_mesh_level_of_detail();
			const auto l_lod = l_mesh_lod.get_lod_culling
			(
				p_context.m_camera.m_main_camera.get_position(),
				p_context.m_camera.m_render_camera.get_position(),
				get_world_position(),
				get_lod_factor()
			);
			if(!l_lod.second)
			{
				return;
			}

			const auto& l_render_states = get_render_states();
			const auto& l_world_transforms = get_world_transforms();
			bc_mesh_utility::render_skinned_mesh(p_context.m_buffer, l_render_states, l_world_transforms, l_lod.first, bc_render_group::dynamic_mesh);
		}

		void bc_skinned_mesh_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			const auto& l_mesh = get_mesh();
			const auto* l_mediate_component = p_context.m_actor.get_component< bc_mediate_component >();
			const auto l_bound_box_z_length = l_mediate_component->get_bound_box().get_half_extends().z;
			auto l_world_transform = l_mediate_component->get_world_transform();
			const auto l_world_offset = l_world_transform.get_rotation() * core::bc_vector3f(0, 0, l_bound_box_z_length * 2.0f);
			l_world_transform.set_translation(l_world_transform.get_translation() + l_world_offset);
			
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
				bc_mesh_component::set_world_transform(p_actor, p_transform);

				const auto& l_mesh = get_mesh();
				auto& l_world_transforms = get_world_transforms();

				bcINT32 l_dummy;
				l_mesh.iterate_over_nodes(l_dummy, [this, &l_mesh, &p_transform, &l_world_transforms](const bc_mesh_node& p_node, bcINT32)
				{
					// Reset absolute transforms so skinned mesh will be rendered correctly in T pose
					l_world_transforms.set_node_transform(p_node, core::bc_matrix4f::scale_matrix(l_mesh.get_mesh_scale()) * p_transform);

					return 0;
				});
			}
		}
	}
}