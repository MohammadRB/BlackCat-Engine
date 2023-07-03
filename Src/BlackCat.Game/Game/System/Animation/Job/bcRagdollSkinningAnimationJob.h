// [25/11/2021 MRB]

#pragma once

#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Animation/Job/bcSkinningTransformAnimationJob.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh;
		
		class BC_GAME_DLL bc_ragdoll_animation_job : public bci_skinning_transform_animation_job
		{
		public:
			/**
			 * \brief Model transforms and collider transforms of skinned mesh component must have been updated
			 * \param p_skeleton
			 * \param p_actor 
			 * \param p_component
			 * \param p_skinning_transforms_ws 
			 */
			bc_ragdoll_animation_job(bc_animation_skeleton& p_skeleton,
				bc_actor p_actor,
				bc_skinned_mesh_component& p_component,
				bc_sub_mesh_mat4_transform& p_skinning_transforms_ws) noexcept;

			bc_ragdoll_animation_job(bc_ragdoll_animation_job&&) noexcept = default;

			~bc_ragdoll_animation_job() override = default;

			bc_ragdoll_animation_job& operator=(bc_ragdoll_animation_job&&) noexcept = default;

			const core::bc_matrix4f& get_world() const noexcept override;

			void set_world(const core::bc_matrix4f& p_world) noexcept override;

			const physics::bc_bound_box& get_bound_box() const noexcept override;

			const bc_sub_mesh_mat4_transform& get_skinning_transforms() const noexcept override;
			
			bool run(const platform::bc_clock::update_param& p_clock) override;

		private:
			bc_actor m_actor;
			const bc_skinned_mesh_component* m_mesh_component;
			bc_sub_mesh_mat4_transform* m_skinning_transforms_ws;
			core::bc_matrix4f m_world;
			physics::bc_bound_box m_bound_box;
		};

		inline const core::bc_matrix4f& bc_ragdoll_animation_job::get_world() const noexcept
		{
			return m_world;
		}

		inline void bc_ragdoll_animation_job::set_world(const core::bc_matrix4f& p_world) noexcept
		{
			m_world = p_world;
		}

		inline const physics::bc_bound_box& bc_ragdoll_animation_job::get_bound_box() const noexcept
		{
			return m_bound_box;
		}

		inline const bc_sub_mesh_mat4_transform& bc_ragdoll_animation_job::get_skinning_transforms() const noexcept
		{
			return *m_skinning_transforms_ws;
		}
	}
}