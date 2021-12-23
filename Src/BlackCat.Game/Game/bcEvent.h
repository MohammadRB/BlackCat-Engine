// [04/22/2021 MRB]

#pragma once

#include "Platform/bcEvent.h"

namespace black_cat
{
	namespace game
	{
		class bc_global_config;

		class bc_event_global_config_changed : public core::bc_app_event
		{
			BC_EVENT(glb_cng)

		public:
			explicit bc_event_global_config_changed(bc_global_config& p_config) noexcept
				: bc_app_event(message_name()),
				m_config(&p_config)
			{
			}

			bc_event_global_config_changed(const bc_event_global_config_changed&) noexcept = default;

			~bc_event_global_config_changed() override = default;

			bc_event_global_config_changed& operator=(const bc_event_global_config_changed&) noexcept = default;

			bc_global_config& get_config() const noexcept
			{
				return *m_config;
			}

		private:
			bc_global_config* m_config;
		};

		class bc_event_game_pause_state : public core::bc_app_event
		{
			BC_EVENT(gam_pus)

		public:
			enum class state : bcBYTE
			{
				paused,
				resumed
			};

		public:
			explicit bc_event_game_pause_state(state p_state)
				: bc_app_event(message_name()),
				m_state(p_state)
			{
			}

			bc_event_game_pause_state(const bc_event_game_pause_state&) = default;

			~bc_event_game_pause_state() override = default;

			bc_event_game_pause_state& operator =(const bc_event_game_pause_state&) = default;

			state get_state() const noexcept
			{
				return m_state;
			}

		private:
			state m_state;
		};

		class bc_event_editor_mode : public core::bc_app_event
		{
			BC_EVENT(edt_mde)

		public:
			explicit bc_event_editor_mode(bool p_editor_mode) noexcept
				: bc_app_event(message_name()),
				m_editor_mode(p_editor_mode)
			{
			}

			bc_event_editor_mode(const bc_event_editor_mode&) noexcept = default;

			~bc_event_editor_mode() override = default;

			bc_event_editor_mode& operator=(const bc_event_editor_mode&) noexcept = default;

			bool get_editor_mode() const noexcept
			{
				return m_editor_mode;
			}

		private:
			bool m_editor_mode;
		};

		class bc_event_scene_change : public core::bc_app_event
		{
			BC_EVENT(scn_chg)
			
		public:
			explicit bc_event_scene_change(const bcCHAR* p_scene_name)
				: bc_app_event(message_name()),
				m_scene_name(p_scene_name)
			{
			}

			bc_event_scene_change(const bc_event_scene_change&) noexcept = default;

			~bc_event_scene_change() override = default;

			bc_event_scene_change& operator=(const bc_event_scene_change&) noexcept = default;

			const bcCHAR* get_scene_name() const noexcept
			{
				return m_scene_name;
			}
		
		private:
			const bcCHAR* m_scene_name;
		};
	}
}