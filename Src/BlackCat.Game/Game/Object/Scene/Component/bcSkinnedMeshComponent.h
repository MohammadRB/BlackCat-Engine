// [12/31/2020 MRB]

#pragma once

#include "Core/Container/bcIteratorAdapter.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Animation/bcSkinnedAnimation.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_skinned_mesh_component : public bc_mesh_component
		{
			BC_COMPONENT(skn_msh, true, false)

		public:
			using animation_iterator = core::bc_const_iterator_adapter<core::bc_vector<bc_skinned_animation_ptr>>;
			
		public:
			bc_skinned_mesh_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_skinned_mesh_component(bc_skinned_mesh_component&& p_other) noexcept;

			~bc_skinned_mesh_component();

			bc_skinned_mesh_component& operator=(bc_skinned_mesh_component&& p_other) noexcept;

			bc_actor get_actor() const noexcept override;

			bc_sub_mesh_mat4_transform& get_model_transforms() noexcept;
			
			const bc_sub_mesh_mat4_transform& get_model_transforms() const noexcept;

			bc_sub_mesh_px_transform& get_collider_model_transforms() noexcept;

			const bc_sub_mesh_px_transform& get_collider_model_transforms() const noexcept;

			animation_iterator get_animations() const noexcept;
			
			void add_animation_job(bci_animation_job& p_animation_job) noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void update(const bc_actor_component_update_content& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;

			void render(const bc_actor_component_render_context& p_context) const override;

			void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;
			
		private:
			void _set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform);

			bc_sub_mesh_mat4_transform m_model_transforms;
			bc_sub_mesh_px_transform m_collider_model_transforms;
			core::bc_vector<bc_skinned_animation_ptr> m_animations;
			bool m_animation_played;
		};

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
		
		inline bc_skinned_mesh_component::animation_iterator bc_skinned_mesh_component::get_animations() const noexcept
		{
			return animation_iterator(m_animations);
		}
	}
}