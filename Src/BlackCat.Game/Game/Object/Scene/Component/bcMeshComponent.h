// [04/29/2016 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcMeshRenderState.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_component : public bc_render_component
		{
			BC_ABSTRACT_COMPONENT(mesh)
			
		public:
			const bc_sub_mesh& get_mesh() const;

			bc_sub_mesh_mat4_transform& get_world_transforms();
			
			const bc_sub_mesh_mat4_transform& get_world_transforms() const;

			core::bc_vector3f get_world_position() const;

			core::bc_matrix4f get_world_transform() const;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void render(const bc_actor_component_render_context& p_context) const override;

		protected:
			bc_mesh_component();

			bc_mesh_component(bc_mesh_component&&) noexcept;

			~bc_mesh_component() override;

			bc_mesh_component& operator=(bc_mesh_component&&) noexcept;
			
			const bc_mesh_render_state& get_render_states() const noexcept;
			
			void set_render_states(bc_mesh_render_state p_render_state) noexcept;
			
			void set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform) noexcept;

			bcFLOAT get_lod_factor() const noexcept;
			
			void update_lod_factor(const physics::bc_bound_box& p_bound_box) noexcept;

		private:
			bc_sub_mesh m_sub_mesh;
			bc_mesh_render_state m_render_state;
			bc_sub_mesh_mat4_transform m_world_transforms;
			bcFLOAT m_lod_scale;
			bcFLOAT m_lod_factor;
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

		inline const bc_mesh_render_state& bc_mesh_component::get_render_states() const noexcept
		{
			return m_render_state;
		}

		inline void bc_mesh_component::set_render_states(bc_mesh_render_state p_render_state) noexcept
		{
			m_render_state = std::move(p_render_state);
		}
		
		inline bcFLOAT bc_mesh_component::get_lod_factor() const noexcept
		{
			return m_lod_factor;
		}
	}
}