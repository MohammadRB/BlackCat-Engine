// [10/09/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "BlackCat/SampleImp/ActorController/bcXBotActorController.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_network_player_actor_controller : public bc_xbot_actor_controller
	{
	public:
		bc_xbot_network_player_actor_controller() noexcept;

		bc_xbot_network_player_actor_controller(bc_xbot_network_player_actor_controller&&) noexcept = default;

		~bc_xbot_network_player_actor_controller() override = default;

		bc_xbot_network_player_actor_controller& operator=(bc_xbot_network_player_actor_controller&&) noexcept = default;
		
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;
		
		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;
		
		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;
		
		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;
		
		void removed_from_scene(const game::bc_actor_component_event_context& p_context,game::bc_scene& p_scene) override;
		
		void handle_event(const game::bc_actor_component_event_context& p_context) override;

	private:
		core::bc_velocity<bcFLOAT> m_look_velocity;
		
		core::bc_vector3f m_network_position;
		core::bc_vector3f m_network_look_direction;
		bcINT32 m_network_look_side;
		bool m_network_forward_pressed;
		bool m_network_backward_pressed;
		bool m_network_right_pressed;
		bool m_network_left_pressed;
		bool m_network_walk_pressed;
	};
}