// [12/24/2021 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Container/bcVector.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Utility/bcServiceManager.h"
#include "Game/bcEvent.h"
#include "BoX.Game/Application/bxDefinitions.h"
#include "BoX.Game/Application/bxGameState.h"
#include "BoX.Game/bxExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_event_manager;
	}
}

namespace box
{
	using namespace black_cat;

	enum class bx_player_state
	{
		ghost,
		team_select,
		playing
	};

	struct bx_player_ui_message
	{
		platform::bc_clock::small_time m_lifetime;
		core::bc_wstring m_message;
	};

	struct bx_player_kill_message
	{
		platform::bc_clock::small_time m_lifetime;
		bx_player_kill_state m_kill;
	};

	class BX_GAME_DLL bx_player_service : public core::bci_service
	{
		BC_SERVICE(ply_srv)

	public:
		explicit bx_player_service(core::bc_event_manager& p_event_manager);

		bx_player_service(bx_player_service&&) = delete;

		~bx_player_service() override = default;

		bx_player_service& operator=(bx_player_service&&) = delete;

		bx_player_state get_state() const noexcept;

		core::bc_vector_frame<core::bc_wstring> get_info_messages() const noexcept;

		core::bc_vector_frame<core::bc_wstring> get_error_messages() const noexcept;

		core::bc_vector_frame<bx_player_kill_state> get_kill_messages() const noexcept;

		bcUINT32 get_game_time() const noexcept;

		void set_game_time(bcUINT32 p_time) noexcept;

		bcINT32 get_health() const noexcept;

		void set_health(bcINT32 p_health) noexcept;

		bcINT32 get_weapon_heat() const noexcept;

		void set_weapon_heat(bcINT32 p_heat) noexcept;

		bcINT32 get_grenade_load() const noexcept;

		void set_grenade_load(bcINT32 p_load) noexcept;

		bcINT32 get_smoke_load() const noexcept;

		void set_smoke_load(bcINT32 p_load) noexcept;

		core::bc_task<bx_team> ask_for_team() noexcept;

		void add_info(core::bc_wstring p_message) noexcept;

		void add_error(core::bc_wstring p_message) noexcept;

		void add_kill(bx_player_kill_state p_kill);

		void started_playing(bx_team p_team) noexcept;

		void stopped_playing() noexcept;

		void update(const platform::bc_clock::update_param& p_clock) override;

	private:
		void _event_handler(core::bci_event& p_event);

		constexpr static bcFLOAT s_msg_lifetime = 4.f;

		bx_player_state m_state;
		core::bc_task_link<bx_team> m_team_task;
		bx_team m_selected_team;
		bcUINT32 m_game_time;
		bcINT32 m_player_health;
		bcINT32 m_weapon_heat;
		bcINT32 m_grenade_load;
		bcINT32 m_smoke_load;

		mutable platform::bc_mutex m_messages_lock;
		core::bc_vector<bx_player_ui_message> m_info_messages;
		core::bc_vector<bx_player_ui_message> m_error_messages;
		core::bc_vector<bx_player_kill_message> m_kill_list;

		core::bc_event_listener_handle m_key_handle;
	};

	inline bx_player_state bx_player_service::get_state() const noexcept
	{
		return m_state;
	}

	inline bcUINT32 bx_player_service::get_game_time() const noexcept
	{
		return m_game_time;
	}

	inline void bx_player_service::set_game_time(bcUINT32 p_time) noexcept
	{
		m_game_time = p_time;
	}

	inline bcINT32 bx_player_service::get_health() const noexcept
	{
		return m_player_health;
	}

	inline void bx_player_service::set_health(bcINT32 p_health) noexcept
	{
		m_player_health = p_health;
	}

	inline bcINT32 bx_player_service::get_weapon_heat() const noexcept
	{
		return m_weapon_heat;
	}

	inline void bx_player_service::set_weapon_heat(bcINT32 p_heat) noexcept
	{
		m_weapon_heat = p_heat;
	}

	inline bcINT32 bx_player_service::get_grenade_load() const noexcept
	{
		return m_grenade_load;
	}

	inline void bx_player_service::set_grenade_load(bcINT32 p_load) noexcept
	{
		m_grenade_load = p_load;
	}

	inline bcINT32 bx_player_service::get_smoke_load() const noexcept
	{
		return m_smoke_load;
	}

	inline void bx_player_service::set_smoke_load(bcINT32 p_load) noexcept
	{
		m_smoke_load = p_load;
	}
}
