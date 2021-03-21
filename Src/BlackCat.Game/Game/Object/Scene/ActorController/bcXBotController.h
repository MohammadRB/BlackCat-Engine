// [01/04/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcVelocity.h"
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
			bc_xbot_controller();

			bc_xbot_controller(bc_xbot_controller&&) = default;
			
			virtual ~bc_xbot_controller() override = default;

			bc_xbot_controller& operator=(bc_xbot_controller&&) = default;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void update(const bc_actor_component_update_content& p_context) override;

			void removed_from_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;

		protected:
			const core::bc_vector3f& get_local_forward() const noexcept;

			bci_animation_job* get_idle_animation() const noexcept;
			
			bci_animation_job* get_running_animation() const noexcept;

			const core::bc_vector3f& get_position() const noexcept;
			
			const core::bc_vector3f& get_look_direction() const noexcept;
			
			const core::bc_vector3f& get_move_direction() const noexcept;
			
			bcFLOAT get_move_speed() const noexcept;
			
			void set_active_animation(bci_animation_job* p_animation) noexcept;

			void set_look_delta(bcINT32 p_x_change) noexcept;

			void set_move_forward(bool p_value) noexcept;

			void set_move_backward(bool p_value) noexcept;

			void set_move_right(bool p_value) noexcept;
			
			void set_move_left(bool p_value) noexcept;

			void set_walk(bool p_value) noexcept;
			
		private:
			void _create_idle_animation(bc_actor& p_actor);

			void _create_running_animation(bc_actor& p_actor);

			void _update_input(const core_platform::bc_clock::update_param& p_clock);

			void _update_direction(const core_platform::bc_clock::update_param& p_clock);

			void _select_active_animation(const core_platform::bc_clock::update_param& p_clock);

			void _update_world_transform(bc_actor& p_actor);

			bc_scene* m_scene;
			bc_skinned_mesh_component* m_skinned_component;
			core::bc_vector<core::bc_string> m_upper_body_chain;
			core::bc_vector3f m_local_forward;

			core::bc_shared_ptr<bc_sampling_animation_job> m_idle_sample_job;
			core::bc_shared_ptr<bc_sampling_animation_job> m_walking_sample_job;
			core::bc_shared_ptr<bc_sampling_animation_job> m_walking_backward_sample_job;
			core::bc_shared_ptr<bc_sampling_animation_job> m_running_sample_job;
			core::bc_shared_ptr<bc_sampling_animation_job> m_running_backward_sample_job;
			core::bc_shared_ptr<bci_animation_job> m_idle_job;
			core::bc_shared_ptr<bci_animation_job> m_running_job;
			bci_animation_job* m_active_job;

			bcFLOAT m_look_speed;
			bcFLOAT m_run_speed;
			bcFLOAT m_walk_speed;
			bcFLOAT m_move_speed;

			bcINT32 m_look_delta_x;
			bool m_forward_pressed;
			bool m_backward_pressed;
			bool m_right_pressed;
			bool m_left_pressed;
			bool m_walk_pressed;
			core::bc_velocity<bcFLOAT> m_look_velocity;
			core::bc_velocity<bcFLOAT> m_forward_velocity;
			core::bc_velocity<bcFLOAT> m_backward_velocity;
			core::bc_velocity<bcFLOAT> m_right_velocity;
			core::bc_velocity<bcFLOAT> m_left_velocity;
			core::bc_velocity<bcFLOAT> m_walk_velocity;

			core::bc_vector3f m_position;
			core::bc_vector3f m_look_direction;
			core::bc_vector3f m_move_direction;
			bcFLOAT m_move_amount;
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

		inline const core::bc_vector3f& bc_xbot_controller::get_position() const noexcept
		{
			return m_position;
		}

		inline const core::bc_vector3f& bc_xbot_controller::get_look_direction() const noexcept
		{
			return m_look_direction;
		}

		inline const core::bc_vector3f& bc_xbot_controller::get_move_direction() const noexcept
		{
			return m_move_direction;
		}

		inline bcFLOAT bc_xbot_controller::get_move_speed() const noexcept
		{
			return m_move_amount;
		}
		
		inline void bc_xbot_controller::set_active_animation(bci_animation_job* p_animation) noexcept
		{
			m_active_job = p_animation;
		}
	}
}