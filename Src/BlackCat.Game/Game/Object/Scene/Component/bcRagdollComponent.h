// [11/13/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_ragdoll_component : public bci_actor_component
		{
			BC_COMPONENT(ragdoll, true, false)
			
		public:
			bc_ragdoll_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_ragdoll_component(bc_ragdoll_component&&) noexcept;

			~bc_ragdoll_component() override;

			bc_ragdoll_component& operator=(bc_ragdoll_component&&) noexcept;

			bc_actor get_actor() const noexcept override;
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;
			
			void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;
			
			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
		};
	}	
}