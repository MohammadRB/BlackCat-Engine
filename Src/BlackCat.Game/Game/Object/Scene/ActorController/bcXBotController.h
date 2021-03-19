// [01/04/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcSamplingAnimationJob.h"
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

			void removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

		protected:
			const core::bc_vector3f& get_local_forward() const noexcept;

			bci_animation_job* get_idle_animation() const noexcept;
			
			bci_animation_job* get_running_animation() const noexcept;

			void set_active_animation(bci_animation_job* p_animation) noexcept;
			
		private:
			void _create_idle_animation(bc_actor& p_actor);

			void _create_running_animation(bc_actor& p_actor);

			bc_skinned_mesh_component* m_skinned_component = nullptr;
			core::bc_vector3f m_local_forward;

			core::bc_shared_ptr<bc_sampling_animation_job> m_idle_sample_job;
			core::bc_shared_ptr<bc_sampling_animation_job> m_running_sample_job;
			core::bc_shared_ptr<bc_sampling_animation_job> m_running_backward_sample_job;
			
			bci_animation_job* m_active_job = nullptr;
			core::bc_shared_ptr<bci_animation_job> m_idle_job;
			core::bc_shared_ptr<bci_animation_job> m_running_job;
		};

		inline const core::bc_vector3f& bc_xbot_controller::get_local_forward() const noexcept
		{
			return m_local_forward;
		}

		inline bci_animation_job* bc_xbot_controller::get_idle_animation() const noexcept
		{
			return m_idle_job.get();
		}

		inline bci_animation_job* bc_xbot_controller::get_running_animation() const noexcept
		{
			return m_running_job.get();
		}

		inline void bc_xbot_controller::set_active_animation(bci_animation_job* p_animation) noexcept
		{
			m_active_job = p_animation;
		}
	}
}