// [02/12/2022 MRB]

#pragma once

#include "Game/System/Network/Server/bcNetworkClient.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/bcEvent.h"

namespace box
{
	using namespace black_cat;

	class bx_player_spawned_event : public core::bc_app_event
	{
		BC_EVENT(ply_spn)

	public:
		/**
		 * \brief local player spawned
		 * \param p_actor 
		 */
		explicit bx_player_spawned_event(game::bc_actor p_actor) noexcept
			: bc_app_event(message_name()),
			m_client_id(game::bc_network_client::invalid_id),
			m_actor(std::move(p_actor))
		{
		}

		/**
		 * \brief network player spawned
		 * \param p_client_id 
		 * \param p_actor 
		 */
		bx_player_spawned_event(game::bc_network_client_id p_client_id, game::bc_actor p_actor) noexcept
			: bc_app_event(message_name()),
			m_client_id(p_client_id),
			m_actor(std::move(p_actor))
		{
		}

		bx_player_spawned_event(const bx_player_spawned_event&) noexcept = default;

		~bx_player_spawned_event() override = default;

		bx_player_spawned_event& operator=(const bx_player_spawned_event&) noexcept = default;

		bool is_local_player() const noexcept
		{
			return m_client_id == game::bc_network_client::invalid_id;
		}

		game::bc_network_client_id get_client_id() const noexcept
		{
			return m_client_id;
		}

		const game::bc_actor& get_actor() const noexcept
		{
			return m_actor;
		}

	private:
		game::bc_network_client_id m_client_id;
		game::bc_actor m_actor;
	};

	class bx_player_killed_event : public core::bc_app_event
	{
		BC_EVENT(ply_kil)

	public:
		/**
		 * \brief local player killed
		 * \param p_killer_client_id 
		 * \param p_actor 
		 */
		bx_player_killed_event(game::bc_network_client_id p_killer_client_id, game::bc_actor p_actor) noexcept
			: bc_app_event(message_name()),
			m_killer_client_id(p_killer_client_id),
			m_client_id(game::bc_network_client::invalid_id),
			m_actor(std::move(p_actor))
		{
		}

		/**
		 * \brief network player killed
		 * \param p_killer_client_id 
		 * \param p_client_id 
		 * \param p_actor 
		 */
		bx_player_killed_event(game::bc_network_client_id p_killer_client_id, game::bc_network_client_id p_client_id, game::bc_actor p_actor) noexcept
			: bc_app_event(message_name()),
			m_killer_client_id(p_killer_client_id),
			m_client_id(p_client_id),
			m_actor(std::move(p_actor))
		{
		}

		bx_player_killed_event(const bx_player_killed_event&) noexcept = default;

		~bx_player_killed_event() override = default;

		bx_player_killed_event& operator=(const bx_player_killed_event&) noexcept = default;

		bool is_local_player() const noexcept
		{
			return m_client_id == game::bc_network_client::invalid_id;
		}

		game::bc_network_client_id get_killer_client_id() const noexcept
		{
			return m_killer_client_id;
		}

		game::bc_network_client_id get_client_id() const noexcept
		{
			return m_client_id;
		}

		const game::bc_actor& get_actor() const noexcept
		{
			return m_actor;
		}

	private:
		game::bc_network_client_id m_killer_client_id;
		game::bc_network_client_id m_client_id;
		game::bc_actor m_actor;
	};

	class bx_player_removed_event : public core::bc_app_event
	{
		BC_EVENT(ply_rmv)

	public:
		/**
		 * \brief local player removed
		 * \param p_actor 
		 */
		explicit bx_player_removed_event(game::bc_actor p_actor) noexcept
			: bc_app_event(message_name()),
			m_client_id(game::bc_network_client::invalid_id),
			m_actor(std::move(p_actor))
		{
		}

		/**
		 * \brief network player removed
		 * \param p_client_id 
		 * \param p_actor 
		 */
		bx_player_removed_event(game::bc_network_client_id p_client_id, game::bc_actor p_actor) noexcept
			: bc_app_event(message_name()),
			m_client_id(p_client_id),
			m_actor(std::move(p_actor))
		{
		}

		bx_player_removed_event(const bx_player_removed_event&) noexcept = default;

		~bx_player_removed_event() override = default;

		bx_player_removed_event& operator=(const bx_player_removed_event&) noexcept = default;

		bool is_local_player() const noexcept
		{
			return m_client_id == game::bc_network_client::invalid_id;
		}

		game::bc_network_client_id get_client_id() const noexcept
		{
			return m_client_id;
		}

		const game::bc_actor& get_actor() const noexcept
		{
			return m_actor;
		}

	private:
		game::bc_network_client_id m_client_id;
		game::bc_actor m_actor;
	};
}
