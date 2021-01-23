// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcLazyContent.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/bcUtility.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_component::bc_mesh_component(bc_actor_component_index p_index)
			: bc_render_component(p_index),
			m_sub_mesh(),
			m_world_transforms()
		{
		}

		bc_mesh_component::bc_mesh_component(bc_mesh_component&& p_other) noexcept
			: bc_render_component(std::move(p_other)),
			m_sub_mesh(std::move(p_other.m_sub_mesh)),
			m_world_transforms(std::move(p_other.m_world_transforms))
		{
		}

		bc_mesh_component::~bc_mesh_component() = default;

		bc_mesh_component& bc_mesh_component::operator=(bc_mesh_component&& p_other) noexcept
		{
			m_sub_mesh = std::move(p_other.m_sub_mesh);
			m_world_transforms = std::move(p_other.m_world_transforms);
			bc_render_component::operator=(std::move(p_other));

			return *this;
		}

		void bc_mesh_component::initialize(bc_actor_component_initialize_context& p_context)
		{
			const auto& l_mesh_name = p_context.m_parameters.get_value_throw< core::bc_string >(constant::g_param_mesh);
			const auto* l_sub_mesh_name = p_context.m_parameters.get_value< core::bc_string >(constant::g_param_sub_mesh);
			const bc_mesh_ptr l_mesh = p_context.m_stream_manager.find_content_throw<bc_mesh>(l_mesh_name.c_str());

			m_sub_mesh = l_sub_mesh_name ? bc_sub_mesh(l_mesh, l_sub_mesh_name->c_str()) : bc_sub_mesh(l_mesh);
			m_world_transforms = bc_sub_mesh_transform(*m_sub_mesh.get_root_node());
		}

		void bc_mesh_component::render(bc_render_state_buffer& p_buffer) const
		{
			const bc_mesh_node* l_root_pointer = m_sub_mesh.get_root_node();
			render_mesh(p_buffer, m_sub_mesh, m_world_transforms, &l_root_pointer, &l_root_pointer + 1, nullptr);
		}

		void bc_mesh_component::set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform)
		{
			physics::bc_bound_box l_bound_box;
			m_sub_mesh.calculate_absolute_transforms(p_transform, m_world_transforms, l_bound_box);

			p_actor.add_event(bc_actor_event_bound_box_changed(l_bound_box));
		}
	}
}