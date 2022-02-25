// [02/24/2022 MRB]

#pragma once

#include "Core/Utility/bcDelegate.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class _bc_run_scene_callback_actor_event : public bc_actor_event
		{
			BC_EVENT(scn_cb)

		public:
			_bc_run_scene_callback_actor_event()
				: bc_actor_event(message_name())
			{
			}

			_bc_run_scene_callback_actor_event(const _bc_run_scene_callback_actor_event&) noexcept = default;

			~_bc_run_scene_callback_actor_event() override = default;

			_bc_run_scene_callback_actor_event& operator=(const _bc_run_scene_callback_actor_event&) noexcept = default;
		};

		class BC_GAME_DLL bc_timing_component : public bci_actor_component
		{
			BC_COMPONENT(icon, true, true)

		public:
			using callback = core::bc_delegate<void(const bc_actor&)>;

		public:
			bc_timing_component(bc_actor_id p_actor_index, bc_actor_component_id p_index);

			bc_timing_component(bc_timing_component&& p_other) noexcept = default;

			~bc_timing_component() override = default;

			bc_timing_component& operator=(bc_timing_component&&) noexcept = default;

			bc_actor get_actor() const noexcept override;

			void set_scene_add_callback(callback p_callback) noexcept;

			void handle_event(const bc_actor_component_event_context& p_context) override;

		private:
			callback m_scene_add_callback;
		};
	}
}
