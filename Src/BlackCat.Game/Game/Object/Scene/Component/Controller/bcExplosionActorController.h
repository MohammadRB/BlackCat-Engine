// [12/19/2020 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_explosion_actor_controller : public bc_iactor_controller
		{
		public:
			void initialize(bc_actor& p_actor) override;

			void added_to_scene(bc_actor& p_actor, bc_scene& p_scene) override;

			void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock) override;

		private:
			bc_scene* m_scene = nullptr;
			bcFLOAT m_light_intensity = 0;
			bcFLOAT m_light_particle_intensity = 0;
			bcFLOAT m_light_radius = 0;
			bcFLOAT m_light_rise_per_second = 0;
			bcFLOAT m_light_lifetime_second = 1.0f;
			bcFLOAT m_age = 0;
		};
	}
}