// [12/31/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
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
			: bc_render_component(p_index),
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
			const auto& l_mesh_name = p_context.m_parameters.get_value_throw< core::bc_string >(constant::g_param_mesh);
			const auto& l_animation_names = p_context.m_parameters.get_value_throw< core::bc_vector< core::bc_any > >(constant::g_param_animations);
			auto l_mesh = p_context.m_stream_manager.find_content_throw< bc_mesh >(l_mesh_name.c_str());
			
			m_mesh = bc_sub_mesh(std::move(l_mesh));
			m_mesh_transform = bc_sub_mesh_transform(*m_mesh.get_root_node());
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

		void bc_skinned_mesh_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			const auto* l_world_transform_event = core::bc_imessage::as<bc_actor_event_world_transform>(p_event);
			if (l_world_transform_event)
			{
				_set_world_transform(p_actor, l_world_transform_event->get_transform());
			}
		}
		
		void bc_skinned_mesh_component::render(bc_render_state_buffer& p_buffer) const
		{
			const bc_mesh_node* l_root_pointer = m_mesh.get_root_node();
			render_skinned_mesh(p_buffer, m_mesh, m_mesh_transform, &l_root_pointer, &l_root_pointer + 1);
		}

		void bc_skinned_mesh_component::debug_draw(const bc_actor& p_actor, bc_shape_drawer& p_shape_drawer)
		{
			p_shape_drawer.draw_wired_skeleton(m_mesh, m_mesh_transform);
		}

		void bc_skinned_mesh_component::_set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform)
		{
			if(m_animation_played) 
			{
				physics::bc_bound_box l_bound_box;
				m_mesh.calculate_bound_box(m_mesh_transform, l_bound_box);

				p_actor.add_event(bc_actor_event_bound_box_changed(l_bound_box));
			}
			else
			{
				// mesh is in bind pose
				physics::bc_bound_box l_bound_box;
				m_mesh.calculate_absolute_transforms(p_transform, m_mesh_transform, l_bound_box);

				p_actor.add_event(bc_actor_event_bound_box_changed(l_bound_box));
			}
		}
	}
}