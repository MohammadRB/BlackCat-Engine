// [24/12/2021 MRB]

#include <iomanip>
#include "Core/Container/bcStringStream.h"
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
		m_smoke_load(100),
		m_show_scores(false),
		m_scores_key_pressed(false)
	{
		m_key_handle = p_event_manager.register_event_listener<platform::bc_app_event_key>
		(
			core::bc_event_manager::delegate_type(*this, &bx_player_service::_event_handler)
		);
	}

	core::bc_vector_frame<core::bc_wstring> bx_player_service::get_info_messages() const noexcept
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			core::bc_vector_frame<core::bc_wstring> l_info_messages;
			l_info_messages.reserve(m_info_messages.size());

			std::transform
			(
				std::begin(m_info_messages),
				std::end(m_info_messages),
				std::back_inserter(l_info_messages),
				[](bx_player_ui_message& p_message)
				{
					return p_message.m_message;
				}
			);

			return l_info_messages;
		}
	}

	core::bc_vector_frame<core::bc_wstring> bx_player_service::get_error_messages() const noexcept
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

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

	core::bc_vector_frame<bx_player_kill_state> bx_player_service::get_kill_messages() const noexcept
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			core::bc_vector_frame<bx_player_kill_state> l_kill_list;
			l_kill_list.reserve(m_kill_list.size());

			std::transform
			(
				std::begin(m_kill_list),
				std::end(m_kill_list),
				std::back_inserter(l_kill_list),
				[](bx_player_kill_message& p_message)
				{
					return p_message.m_kill;
				}
			);

			return l_kill_list;
		}
	}

	core::bc_vector_frame<core::bc_wstring> bx_player_service::get_red_team_scores() const noexcept
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			core::bc_vector_frame<core::bc_wstring> l_score_messages;
			l_score_messages.reserve(m_info_messages.size());

			std::copy
			(
				std::begin(m_red_team_scores),
				std::end(m_red_team_scores),
				std::back_inserter(l_score_messages)
			);

			return l_score_messages;
		}
	}

	core::bc_vector_frame<core::bc_wstring> bx_player_service::get_blue_team_scores() const noexcept
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			core::bc_vector_frame<core::bc_wstring> l_score_messages;
			l_score_messages.reserve(m_info_messages.size());

			std::copy
			(
				std::begin(m_blue_team_scores),
				std::end(m_blue_team_scores),
				std::back_inserter(l_score_messages)
			);

			return l_score_messages;
		}
	}

	bool bx_player_service::get_show_scores() const noexcept
	{
		return m_show_scores || m_scores_key_pressed;
	}

	void bx_player_service::set_show_scores(bool p_show) noexcept
	{
		m_show_scores = p_show;
	}

	core::bc_task<bx_team> bx_player_service::ask_for_team() noexcept
	{
		m_state = bx_player_state::team_select;
		m_team_task = core::bc_task_link<bx_team>([this] { return m_selected_team; });

		return m_team_task.get_task();
	}

	void bx_player_service::add_info(core::bc_wstring p_message) noexcept
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			m_info_messages.push_back({ 0, std::move(p_message) });
		}
	}

	void bx_player_service::add_error(core::bc_wstring p_message) noexcept
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			m_error_messages.push_back({ 0, std::move(p_message) });
		}
	}

	void bx_player_service::add_kill(bx_player_kill_state p_kill)
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			m_kill_list.push_back({ 0, std::move(p_kill) });
		}
	}

	void bx_player_service::update_scores(const bx_game_score& p_scores)
	{
		core::bc_vector<core::bc_wstring> l_red_team_scores;
		core::bc_vector<core::bc_wstring> l_blue_team_scores;
		l_red_team_scores.reserve(p_scores.m_red_team.size() + 1);
		l_blue_team_scores.reserve(p_scores.m_blue_team.size() + 1);

		constexpr auto l_name_width = 15;
		constexpr auto l_count_width = 8;
		core::bc_wstring_stream l_stream;
		l_stream.fill(' ');

		l_stream
			<< std::left
			<< std::setw(l_name_width) << "Name"
			<< std::setw(l_count_width) << "Kills"
			<< std::setw(l_count_width) << "Deaths";
		l_red_team_scores.push_back(l_stream.str());
		l_blue_team_scores.push_back(l_stream.str());

		std::transform
		(
			std::begin(p_scores.m_red_team),
			std::end(p_scores.m_red_team),
			std::back_inserter(l_red_team_scores),
			[&](const bx_player_score& p_score)
			{
				l_stream.str(L"");
				l_stream.clear();
				l_stream
						<< std::left
						<< std::setw(l_name_width) << p_score.m_name.c_str()
						<< std::setw(l_count_width) << p_score.m_kill_count
						<< std::setw(l_count_width) << p_score.m_death_count;

				return l_stream.str();
			}
		);
		std::transform
		(
			std::begin(p_scores.m_blue_team),
			std::end(p_scores.m_blue_team),
			std::back_inserter(l_blue_team_scores),
			[&](const bx_player_score& p_score)
			{
				l_stream.str(L"");
				l_stream.clear();
				l_stream
					<< std::left
					<< std::setw(l_name_width) << p_score.m_name.c_str()
					<< std::setw(l_count_width) << p_score.m_kill_count
					<< std::setw(l_count_width) << p_score.m_death_count;

				return l_stream.str();
			}
		);

		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			m_red_team_scores = std::move(l_red_team_scores);
			m_blue_team_scores = std::move(l_blue_team_scores);
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

	void bx_player_service::update(const platform::bc_clock::update_param& p_clock)
	{
		{
			platform::bc_mutex_guard l_lock(m_messages_lock);

			m_info_messages.erase
			(
				std::remove_if
				(
					std::begin(m_info_messages),
					std::end(m_info_messages),
					[&](bx_player_ui_message& l_msg)
					{
						l_msg.m_lifetime += p_clock.m_elapsed_second;
						return l_msg.m_lifetime > s_msg_lifetime;
					}
				),
				std::end(m_info_messages)
			);
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
			m_kill_list.erase
			(
				std::remove_if
				(
					std::begin(m_kill_list),
					std::end(m_kill_list),
					[&](bx_player_kill_message& l_msg)
					{
						l_msg.m_lifetime += p_clock.m_elapsed_second;
						return l_msg.m_lifetime > s_msg_lifetime;
					}
				),
				std::end(m_kill_list)
			);
		}
	}

	void bx_player_service::game_reset() noexcept
	{
		set_game_time(0);
		set_show_scores(false);
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

			if(l_key_event->get_key() == platform::bc_key::kb_tab)
			{
				if(l_key_event->get_key_state() == platform::bc_key_state::pressed)
				{
					m_scores_key_pressed = true;
				}
				else
				{
					m_scores_key_pressed = false;
				}
			}
		}
	}
}
