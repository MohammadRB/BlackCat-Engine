// [01/01/2022 MRB]

#pragma once

#include "App/SampleImp/XBot/bcXBotNetworkPlayerActorController.h"
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

	class BX_GAME_DLL bx_network_player_actor_controller : public bc_xbot_network_player_actor_controller
	{
	public:
		bx_network_player_actor_controller() noexcept;

		bx_network_player_actor_controller(bx_network_player_actor_controller&&) noexcept = default;

		~bx_network_player_actor_controller() override = default;

		bx_network_player_actor_controller& operator=(bx_network_player_actor_controller&&) noexcept = default;

	private:
		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;

		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;

		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;

		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;

		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;

		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

		core::bc_event_manager* m_event_manager;

		bcUBYTE m_health_recover_per_second;
		bcUBYTE m_health_damage_per_thousands_force;
		bcFLOAT m_health;
	};
}