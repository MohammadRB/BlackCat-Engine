// [12/31/2020 MRB]

#pragma once

#include "Core/Container/bcIteratorAdapter.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
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
			using animation_iterator = core::bc_iterator_adapter<core::bc_vector<bc_skinned_animation_ptr>>;
			
		public:
			explicit bc_skinned_mesh_component(bc_actor_component_index p_index);

			bc_skinned_mesh_component(bc_skinned_mesh_component&& p_other) noexcept;

			~bc_skinned_mesh_component();

			bc_skinned_mesh_component& operator=(bc_skinned_mesh_component&& p_other) noexcept;

			bc_actor get_actor() const noexcept override;

			const bc_sub_mesh& get_mesh() const;
			
			bc_sub_mesh_transform& get_mesh_transforms() noexcept;
			
			const bc_sub_mesh_transform& get_mesh_transforms() const noexcept;
			
			animation_iterator get_animations() noexcept;
			
			void add_animation_job(bci_animation_job& p_animation_job) noexcept;

			void initialize(bc_actor_component_initialize_context& p_context) override;
			
			void update(bc_actor_component_update_content& p_context) override;

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;

			void render(bc_render_state_buffer& p_buffer) const override;

			void debug_draw(const bc_actor& p_actor, bc_shape_drawer& p_shape_drawer) override;
			
		private:
			void _set_world_transform(bc_actor& p_actor, const core::bc_matrix4f& p_transform);

			bc_sub_mesh m_mesh;
			bc_sub_mesh_transform m_mesh_transform;
			core::bc_vector<bc_skinned_animation_ptr> m_animations;
			bool m_animation_played;
		};

		inline const bc_sub_mesh& bc_skinned_mesh_component::get_mesh() const
		{
			return m_mesh;
		}
		
		inline bc_sub_mesh_transform& bc_skinned_mesh_component::get_mesh_transforms() noexcept
		{
			return m_mesh_transform;
		}

		inline const bc_sub_mesh_transform& bc_skinned_mesh_component::get_mesh_transforms() const noexcept
		{
			return m_mesh_transform;
		}

		inline bc_skinned_mesh_component::animation_iterator bc_skinned_mesh_component::get_animations() noexcept
		{
			return animation_iterator(m_animations);
		}
	}
}