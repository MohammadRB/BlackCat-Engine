// [12/19/2020 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_fire_actor_controller : public bci_actor_controller
		{
		public:
			void initialize(bc_actor& p_actor) override;

			void update(const bc_actor_component_update_content& p_context) override;

		private:
			bcFLOAT m_light_intensity = 0;
			bcUINT32 m_num_particles_per_second = 30;
			bcUINT32 m_num_spawned_particles_in_current_second = 0;
		};
	}	
}