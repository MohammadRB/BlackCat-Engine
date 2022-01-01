// [12/24/2021 MRB]

#pragma once

#include "Core/Utility/bcServiceManager.h"

namespace box
{
	using namespace black_cat;

	class bx_player_service : public core::bci_service
	{
		BC_SERVICE(ply_ui)

	public:
		bx_player_service();

		bx_player_service(bx_player_service&&) = delete;

		~bx_player_service() override = default;

		bx_player_service& operator=(bx_player_service&&) = delete;

		bcINT32 get_health() const noexcept;

		void set_health(bcINT32 p_health) noexcept;

		bcINT32 get_weapon_heat() const noexcept;

		void set_weapon_heat(bcINT32 p_heat) noexcept;

		bcINT32 get_grenade_load() const noexcept;

		void set_grenade_load(bcINT32 p_load) noexcept;

		bcINT32 get_smoke_load() const noexcept;

		void set_smoke_load(bcINT32 p_load) noexcept;

	private:
		bcINT32 m_player_health;
		bcINT32 m_weapon_heat;
		bcINT32 m_grenade_load;
		bcINT32 m_smoke_load;
	};

	inline bx_player_service::bx_player_service()
		: m_player_health(100),
		m_weapon_heat(0),
		m_grenade_load(100),
		m_smoke_load(100)
	{
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