// [04/27/2022 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcNullable.h"
#include "PlatformImp/Network/bcNetworkAddress.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/Server/bcNetworkClient.h"
#include "BoX.Game/Application/bxGameState.h"
#include "BoX.Game/Application/bxDefinitions.h"
#include "BoX.Game/bxExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
	}
}

namespace box
{
	using namespace black_cat;

	struct bx_player
	{
		platform::bc_network_address m_address;
		game::bc_network_client_id m_id;
		core::bc_string m_name;

		core::bc_vector3f m_seat;
		core::bc_nullable<bx_team> m_team;
		game::bc_actor m_actor;
		bool m_is_dead = false;
		bcFLOAT m_dead_passed_time = 0;

		bcUINT32 m_kill_count = 0;
		bcUINT32 m_death_count = 0;
	};

	struct bx_player_seat
	{
		game::bc_network_client_id m_client_id;
		core::bc_vector3f m_position;
	};

	class bxi_player_list_service : public core::bci_service
	{
		BC_SERVICE(ply_lst)

	public:
		virtual bool are_in_same_team(game::bc_network_client_id p_player_1, game::bc_network_client_id p_player_2) noexcept = 0;
	};

	class BX_GAME_DLL bx_player_list_service : public bxi_player_list_service
	{
	public:
		bx_player_list_service() noexcept = default;

		bx_player_list_service(bx_player_list_service&&) noexcept = delete;

		~bx_player_list_service() override = default;

		bx_player_list_service& operator=(bx_player_list_service&&) noexcept = delete;

		void scene_changed(game::bc_scene* p_scene);

		bx_player& join_player(const platform::bc_network_address& p_network_address, game::bc_network_client_id p_client_id, core::bc_string p_name);

		void disconnect_player(game::bc_network_client_id p_id);

		bx_player* find_player(game::bc_network_client_id p_id);

		bx_player* player_spawned(game::bc_network_client_id p_id, game::bc_actor p_actor);

		std::pair<bx_player*, bx_player*> player_killed(game::bc_network_client_id p_id, game::bc_network_client_id p_killer_id);

		bx_player* player_removed(game::bc_network_client_id p_id);

		std::pair<bool, core::bc_vector3f> assign_seat(game::bc_network_client_id p_client_id, bx_team p_team);

		core::bc_vector_frame<bx_player*> respawn_dead_players(const platform::bc_clock::update_param& p_clock, bcFLOAT p_respawn_time);

		bx_game_score get_scores() const;

		void reset_players() noexcept;

		bool are_in_same_team(game::bc_network_client_id p_player_1, game::bc_network_client_id p_player_2) noexcept override;

		bcSIZE player_count() const noexcept;

		bcSIZE capacity() const noexcept;

	private:
		std::unordered_map<game::bc_network_client_id, bx_player> m_joined_players;
		core::bc_vector<bx_player_seat> m_red_player_seats;
		core::bc_vector<bx_player_seat> m_blue_player_seats;
	};
}
