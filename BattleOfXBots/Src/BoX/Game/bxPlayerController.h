// [12/22/2021 MRB]

#pragma once

#include "App/SampleImp/XBot/bcXBotPlayerActorController.h"

namespace box
{
	using namespace black_cat;

	class bx_player_controller : public bc_xbot_player_actor_controller
	{
	public:
		bx_player_controller() noexcept = default;

		bx_player_controller(bx_player_controller&&) noexcept = default;

		~bx_player_controller() override = default;

		bx_player_controller& operator=(bx_player_controller&&) noexcept = default;

	private:
	};
}