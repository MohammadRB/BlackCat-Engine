// [01/04/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcModelToSkinnedTransformAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_skeleton_animation;
		class bc_skinned_mesh_component;

		class BC_GAME_DLL bc_xbot_controller : public bci_actor_controller
		{
		public:
			virtual ~bc_xbot_controller() override = default;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void update(const bc_actor_component_update_content& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

		protected:
			const core::bc_vector3f& get_local_forward() const noexcept;

			bci_animation_job* get_idle_animation() const noexcept;

			void set_active_animation(bci_animation_job* p_animation) noexcept;
			
		private:
			bc_skinned_mesh_component* m_skinned_component = nullptr;
			core::bc_vector3f m_local_forward;

			bci_animation_job* m_active_job = nullptr;
			core::bc_unique_ptr<bci_animation_job> m_idle_job;
			bc_model_to_skinned_transform_animation_job* m_idle_model_to_skinned_job = nullptr;
		};

		inline const core::bc_vector3f& bc_xbot_controller::get_local_forward() const noexcept
		{
			return m_local_forward;
		}

		inline bci_animation_job* bc_xbot_controller::get_idle_animation() const noexcept
		{
			return m_idle_job.get();
		}

		inline void bc_xbot_controller::set_active_animation(bci_animation_job* p_animation) noexcept
		{
			m_active_job = p_animation;
		}
	}
}