// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/bcUtility.h"
#include "Core/Content/bcContentStreamManager.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
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

			physics::bc_bound_box l_bound_box;
			m_sub_mesh.calculate_absolute_transforms(core::bc_matrix4f::identity(), m_world_transforms, l_bound_box);

			set_lod_factor(l_bound_box);
		}

		void bc_mesh_component::set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform) noexcept
		{
			physics::bc_bound_box l_bound_box;
			m_sub_mesh.calculate_absolute_transforms(p_transform, m_world_transforms, l_bound_box);

			p_actor.add_event(bc_bound_box_changed_actor_event(l_bound_box));
		}

		void bc_mesh_component::process_bullet_hit(bc_particle_manager& p_particle_manager, 
			const bc_bullet_hit_actor_event& p_event,
			bool p_store_reference_to_bullet)
		{
			auto* l_hit_shape_data = static_cast<bc_px_shape_data*>(p_event.get_hit_shape().get_data());
			BC_ASSERT(l_hit_shape_data);

			if (l_hit_shape_data->m_collision_particle)
			{
				p_particle_manager.spawn_emitter
				(
					l_hit_shape_data->m_collision_particle,
					p_event.get_hit_position(),
					p_event.get_hit_normal(),
					nullptr,
					p_event.get_bullet_mass() / bc_get_global_config().get_bullet_reference_mass()
				);
			}

			if (l_hit_shape_data->m_collision_decal)
			{
				add_decal
				(
					l_hit_shape_data->m_collision_decal,
					p_event.get_hit_position(),
					p_event.get_hit_normal(),
					p_store_reference_to_bullet
						? l_hit_shape_data->m_collider_entry->m_attached_node_index
						: bc_mesh_node::s_invalid_index
				);
			}
		}

		void bc_mesh_component::set_lod_factor(const physics::bc_bound_box& p_bound_box) noexcept
		{
			const auto l_bound_box_half_extends = p_bound_box.get_half_extends();
			const auto l_box_length = std::max({ l_bound_box_half_extends.x, l_bound_box_half_extends.y, l_bound_box_half_extends.z }) * 2;
			m_lod_factor = l_box_length * m_lod_scale * bc_get_global_config().get_lod_global_scale();
		}
	}
}