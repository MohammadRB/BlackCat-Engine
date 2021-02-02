// [09/06/2019 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcCameraInstance.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/bcUtility.h"

namespace black_cat
{
	namespace game
	{
		bc_vegetable_mesh_component::bc_vegetable_mesh_component(bc_actor_component_index p_index)
			: bc_mesh_component(p_index)
		{
		}

		bc_vegetable_mesh_component::bc_vegetable_mesh_component(bc_vegetable_mesh_component&& p_other) noexcept
			: bc_mesh_component(std::move(p_other))
		{
		}

		bc_vegetable_mesh_component::~bc_vegetable_mesh_component() = default;

		bc_vegetable_mesh_component& bc_vegetable_mesh_component::operator=(bc_vegetable_mesh_component&& p_other) noexcept
		{
			bc_mesh_component::operator=(std::move(p_other));
			return *this;
		}

		bc_actor bc_vegetable_mesh_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_vegetable_mesh_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			bc_mesh_component::initialize(p_context);
		}

		void bc_vegetable_mesh_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_actor_event_world_transform>(p_context.m_event);
			if (l_world_transform_event)
			{
				bc_mesh_component::set_world_transform(p_context.m_actor, l_world_transform_event->get_transform());
				return;
			}

			const auto* l_bound_box_event = core::bci_message::as< bc_actor_event_bound_box_changed >(p_context.m_event);
			if (l_bound_box_event)
			{
				bc_mesh_component::update_lod_factor(l_bound_box_event->get_bound_box());
			}
		}
		
		void bc_vegetable_mesh_component::render(const bc_actor_component_render_context& p_context) const
		{
			BC_ASSERT(false);
		}

		void bc_vegetable_mesh_component::render(const bc_actor_component_render_context& p_context, bool p_render_leaf) const
		{
			const bcCHAR* l_mesh_prefix = p_render_leaf ? "leaf." : "trunk.";

			const auto& l_sub_mesh = get_mesh();
			const auto l_mesh_lod = l_sub_mesh.get_mesh_level_of_detail();
			const auto& l_mesh = l_mesh_lod.get_mesh(p_context.m_camera.get_position(), get_world_position(), get_lod_factor());
			const auto& l_mesh_transformation = get_world_transforms();
			const auto* l_root_node = l_sub_mesh.get_root_node();

			render_mesh(p_context.m_buffer, l_mesh, l_mesh_transformation, &l_root_node, &l_root_node + 1, l_mesh_prefix);
		}
	}
}