// [12/31/2020 MRB]

#pragma once

#include "Core/Container/bcSpan.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/System/Animation/bcSkinnedAnimation.h"
#include "Game/System/Animation/bcAnimationJob.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_animation_system;
		class bc_particle_manager;
		
		class BC_GAME_DLL bc_skinned_mesh_component : public bc_mesh_component
		{
			BC_COMPONENT(skn_msh, true, false)

		public:
			bc_skinned_mesh_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

			bc_skinned_mesh_component(bc_skinned_mesh_component&& p_other) noexcept;

			~bc_skinned_mesh_component() override;

			bc_skinned_mesh_component& operator=(bc_skinned_mesh_component&& p_other) noexcept;

			bc_actor get_actor() const noexcept override;

			bc_animation_skeleton* get_skeleton() const noexcept;

			/**
			 * \brief Find animation which its name contains passed string
			 * \param p_name
			 * \return Found animation otherwise null.
			 */
			bc_skeleton_animation* find_animation(const bcCHAR* p_name) const noexcept;

			/**
			 * \brief Find animation which its name contains passed string
			 * \param p_name
			 */
			bc_skeleton_animation& find_animation_throw(const bcCHAR* p_name) const;
			
			core::bc_const_span<bc_skeleton_animation*> get_animations() const noexcept;
			
			bc_sub_mesh_mat4_transform& get_model_transforms() noexcept;
			
			const bc_sub_mesh_mat4_transform& get_model_transforms() const noexcept;

			bc_sub_mesh_px_transform& get_collider_model_transforms() noexcept;

			const bc_sub_mesh_px_transform& get_collider_model_transforms() const noexcept;
			
			void add_animation_job(bci_animation_job* p_animation_job) noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void update(const bc_actor_component_update_content& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;

			void render(const bc_actor_component_render_context& p_context) const override;

			void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;

			void add_decal(const bcCHAR* p_decal_name,
				const core::bc_vector3f& p_world_position,
				const core::bc_vector3f& p_world_direction,
				bc_mesh_node::node_index_t p_attached_node_index) override;
			
		private:
			void _set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform);

			bc_animation_system* m_animation_manager;
			bc_sub_mesh_mat4_transform m_model_transforms;
			bc_sub_mesh_px_transform m_collider_model_transforms;
			core::bc_vector<bc_skinned_animation_ptr> m_animations;
			core::bc_vector_movable<bc_skeleton_animation*> m_all_animations;
			bool m_animation_played;
		};

		inline bc_animation_skeleton* bc_skinned_mesh_component::get_skeleton() const noexcept
		{
			return &m_animations[0]->get_skeleton();
		}
		
		inline core::bc_const_span<bc_skeleton_animation*> bc_skinned_mesh_component::get_animations() const noexcept
		{
			return core::bc_make_span(m_all_animations);
		}
		
		inline bc_sub_mesh_mat4_transform& bc_skinned_mesh_component::get_model_transforms() noexcept
		{
			return m_model_transforms;
		}

		inline const bc_sub_mesh_mat4_transform& bc_skinned_mesh_component::get_model_transforms() const noexcept
		{
			return m_model_transforms;
		}

		inline bc_sub_mesh_px_transform& bc_skinned_mesh_component::get_collider_model_transforms() noexcept
		{
			return m_collider_model_transforms;
		}

		inline const bc_sub_mesh_px_transform& bc_skinned_mesh_component::get_collider_model_transforms() const noexcept
		{
			return m_collider_model_transforms;
		}
	}
}