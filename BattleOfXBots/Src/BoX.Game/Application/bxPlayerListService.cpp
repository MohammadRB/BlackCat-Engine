// [04/27/2022 MRB]

#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "BoX.Game/Application/bxPlayerListService.h"
#include "BoX.Game/Game/bxPlayerSeatComponent.h"

namespace box
{
	void bx_player_list_service::scene_changed(game::bc_scene* p_scene)
	{
		if(!p_scene)
		{
			m_red_player_seats.clear();
			m_blue_player_seats.clear();
		}

		const auto l_player_seat_actors = p_scene->get_scene_graph().get_actors<bx_player_seat_component>();

		for (const auto& l_actor : l_player_seat_actors)
		{
			const auto* l_mediate_component = l_actor.get_component<game::bc_mediate_component>();
			const auto* l_seat_component = l_actor.get_component<bx_player_seat_component>();

			if (l_seat_component->get_team() == bx_team::red)
			{
				m_red_player_seats.push_back({ game::bc_network_client::invalid_id, l_mediate_component->get_position() });
			}
			else
			{
				m_blue_player_seats.push_back({ game::bc_network_client::invalid_id, l_mediate_component->get_position() });
			}
		}

		for (auto& l_joined_client : m_joined_players)
		{
			l_joined_client.second.m_team = nullptr;
		}
	}

	bx_player* bx_player_list_service::find_player(game::bc_network_client_id p_id)
	{
		const auto l_ite = m_joined_players.find(p_id);
		if (l_ite == std::end(m_joined_players))
		{
			return nullptr;
		}

		return &l_ite->second;
	}

	bx_player& bx_player_list_service::join_player(const platform::bc_network_address& p_network_address, game::bc_network_client_id p_client_id, core::bc_string p_name)
	{
		bx_player l_client
		{
			p_network_address,
			p_client_id,
			std::move(p_name)
		};
		const auto l_ite = m_joined_players.insert(std::make_pair(p_client_id, std::move(l_client)));

		return l_ite.first->second;
	}
	
	void bx_player_list_service::remove_player(game::bc_network_client_id p_id)
	{
		const auto l_player_ite = m_joined_players.find(p_id);

		if (l_player_ite->second.m_team.has_value())
		{
			auto& l_seats = *l_player_ite->second.m_team == bx_team::red ? m_red_player_seats : m_blue_player_seats;

			const auto l_seat_ite = std::find_if
			(
				std::begin(l_seats),
				std::end(l_seats),
				[&](const bx_player_seat& p_seat)
				{
					return p_seat.m_client_id == l_player_ite->second.m_id;
				}
			);
			l_seat_ite->m_client_id = game::bc_actor::invalid_id;
		}

		m_joined_players.erase(l_player_ite);
	}

	std::pair<bool, core::bc_vector3f> bx_player_list_service::assign_seat(game::bc_network_client_id p_client_id, bx_team p_team)
	{
		auto& l_seats = p_team == bx_team::red ? m_red_player_seats : m_blue_player_seats;
		const auto l_seat_ite = std::find_if
		(
			std::begin(l_seats),
			std::end(l_seats),
			[](const bx_player_seat& p_seat)
			{
				return p_seat.m_client_id == game::bc_network_client::invalid_id;
			}
		);

		if (l_seat_ite == std::end(l_seats))
		{
			return { false, {} };
		}

		const auto l_client_ite = m_joined_players.find(p_client_id);
		l_client_ite->second.m_team.reset(p_team);
		l_client_ite->second.m_seat = l_seat_ite->m_position;
		l_seat_ite->m_client_id = p_client_id;

		return { true, l_seat_ite->m_position };
	}

	core::bc_vector_frame<bx_player*> bx_player_list_service::respawn_dead_players(const platform::bc_clock::update_param& p_clock, bcFLOAT p_respawn_time)
	{
		core::bc_vector_frame<bx_player*> l_respawned_players;

		for (auto& [l_client_id, l_client] : m_joined_players)
		{
			if (!l_client.m_is_dead)
			{
				continue;
			}

			l_client.m_dead_passed_time += p_clock.m_elapsed_second;

			if (l_client.m_dead_passed_time >= p_respawn_time)
			{
				l_client.m_is_dead = false;
				l_client.m_dead_passed_time = 0;

				l_respawned_players.push_back(&l_client);
			}
		}

		return l_respawned_players;
	}

	void bx_player_list_service::reset_players() noexcept
	{
		for (auto& l_seat : m_blue_player_seats)
		{
			l_seat.m_client_id = game::bc_actor::invalid_id;
		}
		for (auto& l_seat : m_red_player_seats)
		{
			l_seat.m_client_id = game::bc_actor::invalid_id;
		}

		for (auto& [l_client_id, l_player] : m_joined_players)
		{
			l_player.m_seat = {};
			l_player.m_team.reset();
			l_player.m_actor = {};
			l_player.m_is_dead = false;
			l_player.m_dead_passed_time = 0;
		}
	}

	bcSIZE bx_player_list_service::player_count() const noexcept
	{
		return m_joined_players.size();
	}

	bcSIZE bx_player_list_service::capacity() const noexcept
	{
		return m_red_player_seats.size() + m_blue_player_seats.size();
	}

	bool bx_player_list_service::are_in_same_team(game::bc_network_client_id p_player_1, game::bc_network_client_id p_player_2) noexcept
	{
		auto* l_player_1 = find_player(p_player_1);
		auto* l_player_2 = find_player(p_player_2);

		return l_player_1 &&
			   l_player_2 && 
			   l_player_1->m_team.has_value() &&
			   l_player_2->m_team.has_value() &&
			   *l_player_1->m_team == *l_player_2->m_team;
	}
}
