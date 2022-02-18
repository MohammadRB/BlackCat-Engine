// [12/24/2021 MRB]

#include "Core/Messaging/Event/bcEventManager.h"
#include "BoX.Game/Application/bxPlayerService.h"

namespace box
{
	bx_player_service::bx_player_service(core::bc_event_manager& p_event_manager)
		: m_state(bx_player_state::ghost),
		m_team_task([this] { return m_selected_team; }),
		m_selected_team(bx_team::red),
		m_game_time(0),
		m_player_health(100),
		m_weapon_heat(0),
		m_grenade_load(100),
		m_smoke_load(100)
	{
		m_key_handle = p_event_manager.register_event_listener<platform::bc_app_event_key>
		(
			core::bc_event_manager::delegate_type(*this, &bx_player_service::_event_handler)
		);
	}

	core::bc_vector_frame<core::bc_wstring> bx_player_service::get_error_messages() const noexcept
	{
		{
			core_platform::bc_mutex_guard l_lock(m_messages_lock);

			core::bc_vector_frame<core::bc_wstring> l_error_messages;
			l_error_messages.reserve(m_error_messages.size());

			std::transform
			(
				std::begin(m_error_messages),
				std::end(m_error_messages),
				std::back_inserter(l_error_messages),
				[](bx_player_ui_message& p_message)
				{
					return p_message.m_message;
				}
			);

			return l_error_messages;
		}
	}

	core::bc_task<bx_team> bx_player_service::ask_for_team() noexcept
	{
		m_state = bx_player_state::team_select;
		m_team_task = core::bc_task_link<bx_team>([this] { return m_selected_team; });

		return m_team_task.get_task();
	}

	void bx_player_service::add_error(core::bc_wstring p_error) noexcept
	{
		{
			core_platform::bc_mutex_guard l_lock(m_messages_lock);

			m_error_messages.push_back({ 0, std::move(p_error) });
		}
	}

	void bx_player_service::started_playing(bx_team p_team) noexcept
	{
		m_state = bx_player_state::playing;
	}

	void bx_player_service::stopped_playing() noexcept
	{
		m_state = bx_player_state::ghost;
	}

	void bx_player_service::update(const core_platform::bc_clock::update_param& p_clock)
	{
		{
			core_platform::bc_mutex_guard l_lock(m_messages_lock);

			m_error_messages.erase
			(
				std::remove_if
				(
					std::begin(m_error_messages),
					std::end(m_error_messages),
					[&](bx_player_ui_message& l_msg)
					{
						l_msg.m_lifetime += p_clock.m_elapsed_second;
						return l_msg.m_lifetime > s_msg_lifetime;
					}
				),
				std::end(m_error_messages)
			);
		}
	}

	void bx_player_service::_event_handler(core::bci_event& p_event)
	{
		if (const auto* l_key_event = core::bci_message::as<platform::bc_app_event_key>(p_event))
		{
			if (m_state == bx_player_state::team_select && l_key_event->get_key_state() == platform::bc_key_state::pressing)
			{
				if (l_key_event->get_key() == platform::bc_key::kb_B)
				{
					m_selected_team = bx_team::blue;
					m_team_task();
				}
				else if (l_key_event->get_key() == platform::bc_key::kb_R)
				{
					m_selected_team = bx_team::red;
					m_team_task();
				}
			}
		}
	}
}