// [24/02/2022 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct bc_actor_component_write_context;
		struct bc_actor_component_load_context;
		struct bc_actor_component_network_write_context;
		struct bc_actor_component_network_load_context;
		class bci_actor_component;
		class bc_actor;

		void BC_GAME_DLL bc_actor_load_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_load_context& p_context);

		void BC_GAME_DLL bc_actor_write_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_write_context& p_context);

		void BC_GAME_DLL bc_actor_load_network_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_network_load_context& p_context);

		void BC_GAME_DLL bc_actor_write_network_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_network_write_context& p_context);

		void BC_GAME_DLL bc_actor_mark_for_double_update(bc_actor& p_actor);

		void BC_GAME_DLL bc_actor_mark_for_checkpoint(bc_actor& p_actor);

		void BC_GAME_DLL bc_actor_unmark_for_checkpoint(bc_actor& p_actor);
	}
}
