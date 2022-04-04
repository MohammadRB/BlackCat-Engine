// [04/29/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcDecalResolverComponent.h"
#include "Game/Object/Scene/Component/bcPickupProxyComponent.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcMeshRenderState.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_physics_system;
		class bc_particle_manager_container;
		
		class BC_GAME_DLL bc_mesh_component : public bc_render_component, public bc_decal_resolver_component, public bc_pickup_proxy_component
		{
			BC_ABSTRACT_COMPONENT(mesh)
			
		public:
			const bc_sub_mesh& get_mesh() const;

			bc_sub_mesh_mat4_transform& get_world_transforms();
			
			const bc_sub_mesh_mat4_transform& get_world_transforms() const;

			core::bc_vector3f get_world_position() const;

			core::bc_matrix4f get_world_transform() const;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void load_instance(const bc_actor_component_load_context& p_context) override;

			void write_instance(const bc_actor_component_write_context& p_context) override;

			bc_pickup_proxy_result ray_pickup(const physics::bc_ray& p_ray) const override;

		protected:
			bc_mesh_component() noexcept;

			bc_mesh_component(bc_mesh_component&&) noexcept;

			~bc_mesh_component() override;

			bc_mesh_component& operator=(bc_mesh_component&&) noexcept;

			bcFLOAT get_view_distance() const noexcept;

			void update_view_distance(const physics::bc_bound_box& p_bound_box) noexcept;
			
			const bc_mesh_render_state& get_render_states() const noexcept;
			
			void set_render_states(bc_mesh_render_state p_render_state) noexcept;
			
			void set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform) noexcept;

			void process_bullet_hit(const bc_physics_system& p_physics_system,
				bc_particle_manager_container& p_particle_manager, 
				const bc_bullet_hit_actor_event& p_event,
				bool p_store_reference_to_bullet);
		
		private:
			bc_sub_mesh m_sub_mesh;
			bcFLOAT m_view_distance;
			bc_mesh_render_state m_render_state;
			bc_sub_mesh_mat4_transform m_world_transforms;
		};

		inline const bc_sub_mesh& bc_mesh_component::get_mesh() const
		{
			return m_sub_mesh;
		}

		inline bc_sub_mesh_mat4_transform& bc_mesh_component::get_world_transforms()
		{
			return m_world_transforms;
		}
		
		inline const bc_sub_mesh_mat4_transform& bc_mesh_component::get_world_transforms() const
		{
			return m_world_transforms;
		}

		inline core::bc_vector3f bc_mesh_component::get_world_position() const
		{
			return m_world_transforms.get_node_transform(*m_sub_mesh.get_root_node()).get_translation();
		}

		inline core::bc_matrix4f bc_mesh_component::get_world_transform() const
		{
			return m_world_transforms.get_node_transform(*m_sub_mesh.get_root_node());
		}

		inline bcFLOAT bc_mesh_component::get_view_distance() const noexcept
		{
			return m_view_distance;
		}
		
		inline const bc_mesh_render_state& bc_mesh_component::get_render_states() const noexcept
		{
			return m_render_state;
		}

		inline void bc_mesh_component::set_render_states(bc_mesh_render_state p_render_state) noexcept
		{
			m_render_state = std::move(p_render_state);
		}
	}
}