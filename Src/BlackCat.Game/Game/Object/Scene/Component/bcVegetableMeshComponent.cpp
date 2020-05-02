// [09/06/2019 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"

namespace black_cat
{
	namespace game
	{
		extern void _render_mesh_node(bc_render_state_buffer& p_buffer,
			const bc_sub_mesh& p_mesh_part,
			const bc_sub_mesh_transformation& p_transformations,
			const bc_mesh_node* p_begin,
			const bc_mesh_node* p_end,
			const bcCHAR* p_mesh_prefix);

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

		void bc_vegetable_mesh_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			bc_mesh_component::initialize(p_actor, p_parameters);
		}

		void bc_vegetable_mesh_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			auto* l_world_transform_event = core::bc_event::event_as<bc_actor_event_world_transform>(p_event);
			if (l_world_transform_event)
			{
				bc_mesh_component::set_world_transform(p_actor, l_world_transform_event->get_transform());
			}
		}
		
		void bc_vegetable_mesh_component::render(bc_render_state_buffer& p_buffer) const
		{
			bcAssert(false);
		}

		void bc_vegetable_mesh_component::render(bc_render_state_buffer& p_buffer, bool p_render_leaf) const
		{
			const bcCHAR* l_mesh_prefix = p_render_leaf ? "leaf_" : "trunk_";

			const auto& l_mesh = get_mesh();
			const auto& l_mesh_transformation = get_mesh_transformation();
			const auto* l_root_node = l_mesh.get_root_node();

			_render_mesh_node(p_buffer, l_mesh, l_mesh_transformation, l_root_node, l_root_node + 1, l_mesh_prefix);
		}
	}
}