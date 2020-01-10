// [09/06/2019 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcVegetableMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		extern void _render_mesh_node(bc_render_system& p_render_system,
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

		void bc_vegetable_mesh_component::render(bc_render_system& p_render_system) const
		{
			bcAssert(false);
		}

		void bc_vegetable_mesh_component::render(bc_render_system& p_render_system, bool p_render_leaf) const
		{
			const bcCHAR* l_mesh_prefix = p_render_leaf ? "leaf_" : "trunk_";

			const auto& l_mesh = get_mesh();
			const auto& l_mesh_transformation = get_mesh_transformation();
			const auto* l_root_node = l_mesh.get_root_node();

			_render_mesh_node(p_render_system, l_mesh, l_mesh_transformation, l_root_node, l_root_node + 1, l_mesh_prefix);
		}

		void bc_vegetable_mesh_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		void bc_vegetable_mesh_component::handle_event(const bc_actor& p_actor, const bc_actor_event& p_event)
		{
		}
	}
}