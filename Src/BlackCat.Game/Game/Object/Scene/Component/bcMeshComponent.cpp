// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/bcUtility.h"
#include "Core/Content/bcContentStreamManager.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_component::bc_mesh_component() noexcept
			: bc_render_component(),
			bc_decal_resolver_component(),
			m_sub_mesh(),
			m_world_transforms(),
			m_lod_scale(0),
			m_lod_factor(0)
		{
		}

		bc_mesh_component::bc_mesh_component(bc_mesh_component&& p_other) noexcept
			: bc_render_component(std::move(p_other)),
			bc_decal_resolver_component(std::move(p_other)),
			m_sub_mesh(std::move(p_other.m_sub_mesh)),
			m_world_transforms(std::move(p_other.m_world_transforms)),
			m_lod_scale(0),
			m_lod_factor(p_other.m_lod_factor)
		{
		}

		bc_mesh_component::~bc_mesh_component() = default;

		bc_mesh_component& bc_mesh_component::operator=(bc_mesh_component&& p_other) noexcept
		{
			bc_render_component::operator=(std::move(p_other));
			bc_decal_resolver_component::operator=(std::move(p_other));
			m_sub_mesh = std::move(p_other.m_sub_mesh);
			m_world_transforms = std::move(p_other.m_world_transforms);
			m_lod_scale = p_other.m_lod_scale;
			m_lod_factor = p_other.m_lod_factor;

			return *this;
		}

		void bc_mesh_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto& l_mesh_name = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_mesh);
			const auto* l_sub_mesh_name = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_sub_mesh);
			const auto* l_materials = p_context.m_parameters.get_value<core::bc_json_key_value>(constant::g_param_mesh_materials);
			const auto l_lod_scale = bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_mesh_lod_scale), 1.0f);
			const auto l_mesh = p_context.m_stream_manager.find_content_throw<bc_mesh>(l_mesh_name.c_str());

			m_sub_mesh = l_sub_mesh_name ? bc_sub_mesh(l_mesh, l_sub_mesh_name->c_str()) : bc_sub_mesh(l_mesh);
			m_lod_scale = l_lod_scale;
			m_world_transforms = bc_sub_mesh_mat4_transform(*m_sub_mesh.get_root_node());

			if(l_materials)
			{
				m_render_state = m_sub_mesh.create_render_states(p_context.m_game_system.get_render_system(), *l_materials);
			}
			else
			{
				m_render_state = m_sub_mesh.create_render_states(p_context.m_game_system.get_render_system());
			}
		}

		void bc_mesh_component::add_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_world_position,
			const core::bc_vector3f& p_dir,
			bc_mesh_node::node_index_t p_attached_node_index)
		{
			const auto l_world_transform_node = p_attached_node_index != bc_mesh_node::s_invalid_index
				? p_attached_node_index
				: m_sub_mesh.get_root_node()->get_transform_index();
			const auto& l_node_world_transform = m_world_transforms[l_world_transform_node];
			const auto l_inv_node_world_transform = l_node_world_transform.inverse();

			const auto l_local_position = (l_inv_node_world_transform * core::bc_vector4f(p_world_position, 1)).xyz();
			const auto l_local_direction = core::bc_vector3f::normalize((l_inv_node_world_transform * core::bc_vector4f(p_dir, 0)).xyz());
			core::bc_matrix3f l_local_rotation;
			
			if (graphic::bc_render_api_info::use_left_handed())
			{
				l_local_rotation.rotation_between_two_vector_lh(l_local_direction, core::bc_vector3f::up());
			}
			else
			{
				l_local_rotation.rotation_between_two_vector_rh(l_local_direction, core::bc_vector3f::up());
			}

			auto l_actor = get_actor();
			auto* l_decal_component = l_actor.get_create_component<bc_decal_component>();
			auto* l_rigid_body_component = l_actor.get_component<bc_rigid_body_component>();

			if(!l_rigid_body_component || l_rigid_body_component->get_body().get_type() == physics::bc_actor_type::rigid_static)
			{
				//p_attached_node_index = bc_mesh_node::s_invalid_index;
			}
			
			l_decal_component->add_decal(p_decal_name, l_local_position, l_local_rotation, l_node_world_transform, p_attached_node_index);
		}

		void bc_mesh_component::set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform) noexcept
		{
			physics::bc_bound_box l_bound_box;
			m_sub_mesh.calculate_absolute_transforms(p_transform, m_world_transforms, l_bound_box);

			p_actor.add_event(bc_bound_box_changed_actor_event(l_bound_box));
		}

		void bc_mesh_component::update_lod_factor(const physics::bc_bound_box& p_bound_box) noexcept
		{
			const auto l_bound_box_half_extends = p_bound_box.get_half_extends();
			const auto l_box_length = std::max(std::max(l_bound_box_half_extends.x, l_bound_box_half_extends.y), l_bound_box_half_extends.z) * 2;
			m_lod_factor = l_box_length * m_lod_scale * get_global_config().get_lod_global_scale();
		}
	}
}