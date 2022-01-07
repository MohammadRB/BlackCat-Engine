// [12/22/2021 MRB]

#pragma once

#include "App/SampleImp/XBot/bcXBotPlayerActorController.h"

namespace box
{
	using namespace black_cat;
	class bx_player_service;

	class bx_player_actor_controller : public bc_xbot_player_actor_controller
	{
	public:
		bx_player_actor_controller() noexcept;

		bx_player_actor_controller(bx_player_actor_controller&&) noexcept = default;

		~bx_player_actor_controller() override = default;

		bx_player_actor_controller& operator=(bx_player_actor_controller&&) noexcept = default;

	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;

		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;

		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;

		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;

		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;

		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

		bool can_look() noexcept override;

		bool can_move() noexcept override;

		bool can_throw_grenade(core::bc_string_view p_entity) noexcept override;

		bool can_attach_weapon(core::bc_string_view p_entity) noexcept override;

		bool can_shoot_weapon() noexcept override;

		void grenade_thrown(core::bc_string_view p_entity) noexcept override;

		void weapon_attached(game::bc_actor& p_weapon) noexcept override;

		void weapon_shoot(game::bc_actor& p_weapon) noexcept override;
		
		bx_player_service* m_ui_service;

		bcUBYTE m_health_recover_per_second;
		bcUBYTE m_health_damage_per_thousands_force;
		bcUBYTE m_rifle_heat_per_shoot;
		bcUBYTE m_rifle_cool_per_second;
		bcUBYTE m_grenade_load_time;
		bcUBYTE m_smoke_load_time;

		bcFLOAT m_health;
		bcFLOAT m_rifle_heat;
		bcFLOAT m_grenade_load;
		bcFLOAT m_smoke_load;
	};
}