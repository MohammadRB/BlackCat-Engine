// [06/06/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/bcUtility.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_component::bc_mesh_component() noexcept
			: bc_render_component(),
			bc_decal_resolver_component(),
			m_view_distance(0)
		{
		}

		bc_mesh_component::bc_mesh_component(bc_mesh_component&& p_other) noexcept
			: bc_render_component(std::move(p_other)),
			bc_decal_resolver_component(std::move(p_other)),
			m_sub_mesh(std::move(p_other.m_sub_mesh)),
			m_world_transforms(std::move(p_other.m_world_transforms)),
			m_view_distance(p_other.m_view_distance)
		{
		}

		bc_mesh_component::~bc_mesh_component() = default;

		bc_mesh_component& bc_mesh_component::operator=(bc_mesh_component&& p_other) noexcept
		{
			bc_render_component::operator=(std::move(p_other));
			bc_decal_resolver_component::operator=(std::move(p_other));
			m_sub_mesh = std::move(p_other.m_sub_mesh);
			m_world_transforms = std::move(p_other.m_world_transforms);
			m_view_distance = p_other.m_view_distance;

			return *this;
		}

		void bc_mesh_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const core::bc_string* l_mesh_name = nullptr;
			const core::bc_string* l_sub_mesh_name = nullptr;
			const core::bc_json_key_value* l_materials = nullptr;
			bcFLOAT l_view_distance_param = 0;

			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_mesh, l_mesh_name);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_sub_mesh, l_sub_mesh_name);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_mesh_materials, l_materials);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_mesh_view_distance, l_view_distance_param);

			const auto l_mesh = p_context.m_stream_manager.find_content_throw<bc_mesh>(l_mesh_name->c_str());
			m_sub_mesh = l_sub_mesh_name ? bc_sub_mesh(l_mesh, l_sub_mesh_name->c_str()) : bc_sub_mesh(l_mesh);
			m_world_transforms = bc_sub_mesh_mat4_transform(*m_sub_mesh.get_root_node());
			m_view_distance = l_view_distance_param;

			if(l_materials)
			{
				m_render_state = m_sub_mesh.create_render_states(p_context.m_game_system.get_render_system(), *l_materials);
			}
			else
			{
				m_render_state = m_sub_mesh.create_render_states(p_context.m_game_system.get_render_system());
			}

			if(m_view_distance <= 0)
			{
				physics::bc_bound_box l_bound_box;
				m_sub_mesh.calculate_absolute_transforms(core::bc_matrix4f::identity(), m_world_transforms, l_bound_box);

				update_view_distance(l_bound_box);
			}

			// Create decal component in advance to prevent component creation in 'add_decal' implementations
			// which will be called during concurrent event processing
			p_context.m_actor.create_component<bc_decal_component>();
		}

		void bc_mesh_component::load_instance(const bc_actor_component_load_context& p_context)
		{
			bc_decal_resolver_component::load_instance(p_context);
		}

		void bc_mesh_component::write_instance(const bc_actor_component_write_context& p_context)
		{
			bc_decal_resolver_component::write_instance(p_context);
		}

		bc_pickup_proxy_result bc_mesh_component::ray_pickup(const physics::bc_ray& p_ray) const
		{
			auto* l_rigid_body_component = get_actor().get_component<bc_rigid_body_component>();
			if(!l_rigid_body_component)
			{
				return { bc_pickup_proxy_state::no_result, physics::bc_ray_hit() };
			}

			{
				bc_rigid_component_shared_lock l_lock(*l_rigid_body_component);

				const auto l_rigid_body = l_rigid_body_component->get_body();
				const auto l_shape_count = l_rigid_body.get_shape_count();
				auto l_shapes = core::bc_vector_frame<physics::bc_shape>(l_shape_count);
				auto l_ray_hit = physics::bc_ray_hit();

				l_rigid_body.get_shapes(l_shapes.data(), l_shape_count);

				for (auto& l_shape : l_shapes)
				{
					core::bc_any l_shape_container;
					physics::bc_shape_geometry* l_shape_geometry = nullptr;

					switch (l_shape.get_type())
					{
					case physics::bc_shape_type::sphere:
					{
						auto [l_is, l_sphere] = l_shape.as_sphere();
						l_shape_container.set_value(l_sphere);
						l_shape_geometry = l_shape_container.as<physics::bc_shape_sphere>();
						break;
					}
					case physics::bc_shape_type::plane:
					{
						auto [l_is, l_plane] = l_shape.as_plane();
						l_shape_container.set_value(l_plane);
						l_shape_geometry = l_shape_container.as<physics::bc_shape_plane>();
						break;
					}
					case physics::bc_shape_type::capsule:
					{
						auto [l_is, l_capsule] = l_shape.as_capsule();
						l_shape_container.set_value(l_capsule);
						l_shape_geometry = l_shape_container.as<physics::bc_shape_capsule>();
						break;
					}
					case physics::bc_shape_type::box:
					{
						auto [l_is, l_box] = l_shape.as_box();
						l_shape_container.set_value(l_box);
						l_shape_geometry = l_shape_container.as<physics::bc_shape_box>();
						break;
					}
					case physics::bc_shape_type::convex_mesh:
					{
						auto [l_is, l_convex] = l_shape.as_convex_mesh();
						l_shape_container.set_value(l_convex);
						l_shape_geometry = l_shape_container.as<physics::bc_shape_convex_mesh>();
						break;
					}
					case physics::bc_shape_type::triangle_mesh:
					{
						auto [l_is, l_triangle_mesh] = l_shape.as_triangle_mesh();
						l_shape_container.set_value(l_triangle_mesh);
						l_shape_geometry = l_shape_container.as<physics::bc_shape_triangle_mesh>();
						break;
					}
					}

					if (!l_shape_geometry)
					{
						continue;
					}

					const bool l_intersects = physics::bc_shape_query::ray_cast
					(
						p_ray,
						*l_shape_geometry,
						l_rigid_body.get_global_pose() * l_shape.get_local_pose(),
						physics::bc_hit_flag::distance,
						&l_ray_hit,
						1
					);

					if (l_intersects)
					{
						return { bc_pickup_proxy_state::pickup, l_ray_hit };
					}
				}

				return { bc_pickup_proxy_state::no_pickup, l_ray_hit };
			}
		}

		void bc_mesh_component::set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform) noexcept
		{
			physics::bc_bound_box l_bound_box;
			m_sub_mesh.calculate_absolute_transforms(p_transform, m_world_transforms, l_bound_box);

			p_actor.add_event(bc_bound_box_changed_actor_event(l_bound_box));
		}

		void bc_mesh_component::process_bullet_hit(const bc_physics_system& p_physics_system,
			bc_particle_manager_container& p_particle_manager,
			const bc_bullet_hit_actor_event& p_event,
			bool p_store_reference_to_bullet)
		{
			auto* l_hit_shape_data = p_physics_system.get_game_shape_data(p_event.get_hit_shape());

			BC_ASSERT(l_hit_shape_data);

			if (l_hit_shape_data->m_collision_particle)
			{
				p_particle_manager.spawn_emitter
				(
					l_hit_shape_data->m_collision_particle,
					p_event.get_hit_position(),
					p_event.get_hit_normal(),
					nullptr,
					p_event.get_bullet_mass() / bc_get_global_config().get_scene_bullet_reference_mass()
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

		void bc_mesh_component::update_view_distance(const physics::bc_bound_box& p_bound_box) noexcept
		{
			const auto l_bound_box_half_extends = p_bound_box.get_half_extends();
			const auto l_box_length = std::max({ l_bound_box_half_extends.x, l_bound_box_half_extends.y, l_bound_box_half_extends.z }) * 2;
			m_view_distance = l_box_length * 30 * bc_get_global_config().get_scene_global_view_distance_scale();
		}
	}
}