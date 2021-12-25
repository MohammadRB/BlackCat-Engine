// [12/24/2021 MRB]

#pragma once

#include "Core/Utility/bcServiceManager.h"

namespace box
{
	using namespace black_cat;

	class bx_player_ui_service : public core::bci_service
	{
		BC_SERVICE(ply_ui)

	public:
		bx_player_ui_service();

		bx_player_ui_service(bx_player_ui_service&&) = delete;

		~bx_player_ui_service() override = default;

		bx_player_ui_service& operator=(bx_player_ui_service&&) = delete;

	private:
		bcINT32 m_player_health;
		bcINT32 m_rifle_heat;
		bcINT32 m_grenade_load;
		bcINT32 m_smoke_load;
	};
}