// [10/16/2021 MRB]

#pragma once

#include "BlackCat/SampleImp/XBot/bcXBotActorController.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_idle_actor_controller : public bc_xbot_actor_controller
	{
	public:
		void load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context) override;
		
		void write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context) override;
		
		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;
		
		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;
		
	private:
		void throw_grenade(game::bc_actor& p_grenade) noexcept override;
	};

	inline void bc_xbot_idle_actor_controller::load_origin_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::load_replicated_network_instance(const game::bc_actor_component_network_load_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::write_origin_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::write_replicated_network_instance(const game::bc_actor_component_network_write_context& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::update_origin_instance(const game::bc_actor_component_update_content& p_context)
	{
		bc_xbot_actor_controller::update(bc_xbot_input_update_context
		{
			p_context.m_clock,
			0,
			false,
			false,
			false,
			false,
			false
		});
	}

	inline void bc_xbot_idle_actor_controller::update_replicated_instance(const game::bc_actor_component_update_content& p_context)
	{
	}

	inline void bc_xbot_idle_actor_controller::throw_grenade(game::bc_actor& p_grenade) noexcept
	{
	}
}