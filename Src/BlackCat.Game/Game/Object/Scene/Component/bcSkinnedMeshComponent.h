// [12/31/2020 MRB]

#pragma once

#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransformation.h"
#include "Game/Object/Animation/bcSkinnedAnimation.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_skinned_mesh_component : public bc_render_component
		{
			BC_COMPONENT(skn_msh, true, false)
			
		public:
			explicit bc_skinned_mesh_component(bc_actor_component_index p_index);

			bc_skinned_mesh_component(bc_skinned_mesh_component&& p_other) noexcept;

			~bc_skinned_mesh_component();

			bc_skinned_mesh_component& operator=(bc_skinned_mesh_component&& p_other) noexcept;

			bc_actor get_actor() const noexcept override;
					
			void add_animation_job(bci_animation_job& p_animation_job) noexcept;

			void initialize(bc_actor_component_initialize_context& p_context) override;
			
			void update(bc_actor_component_update_content& p_context) override;

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;

			void render(bc_render_state_buffer& p_buffer) const override;

		private:
			void _set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform);
			
			bc_sub_mesh m_mesh;
			bc_sub_mesh_transformation m_mesh_transformation;
			core::bc_vector<bc_skinned_animation_ptr> m_animations;
			core::bc_vector_movable<bc_skeleton_animation*> m_all_animations;
			bool m_animation_played;
		};
	}
}