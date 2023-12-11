// [07/04/2022 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_controller_component : public bci_actor_component
	{
		BC_COMPONENT(control, true, true)

	public:
		bc_controller_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_controller_component(bc_controller_component&&) noexcept = default;

		~bc_controller_component() override = default;

		bc_controller_component& operator=(bc_controller_component&&) noexcept = default;

		bci_actor_controller* get_controller() const noexcept;

		void set_controller(core::bc_unique_ptr<bci_actor_controller> p_controller, const bc_actor_component_initialize_context& p_context) noexcept;
		
		inline void load_instance(const bc_actor_component_load_context& p_context) override;

		inline void write_instance(const bc_actor_component_write_context& p_context) override;

		inline void load_network_instance(const bc_actor_component_network_load_context& p_context) override;

		inline void write_network_instance(const bc_actor_component_network_write_context& p_context) override;

		inline void update(const bc_actor_component_update_content& p_context) override;

		inline void handle_event(const bc_actor_component_event_context& p_context) override;

		inline void debug_draw(const bc_actor_component_debug_draw_context& p_context) override;

	private:
		core::bc_unique_ptr<bci_actor_controller> m_controller;
	};

	inline bci_actor_controller* bc_controller_component::get_controller() const noexcept
	{
		return m_controller.get();
	}
}
