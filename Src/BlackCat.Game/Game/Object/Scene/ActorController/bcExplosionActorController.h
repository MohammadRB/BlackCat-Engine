// [12/19/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Query/bcSceneQuery.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_explosion_actor_controller : public bci_actor_controller
		{
		public:
			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene) override;

			void update(const bc_actor_component_update_content& p_context) override;

		private:
			bcFLOAT m_light_intensity = 0;
			bcFLOAT m_light_particle_intensity = 0;
			bcFLOAT m_light_radius = 0;
			bcFLOAT m_light_rise_per_second = 0;
			bcFLOAT m_light_lifetime_second = 1.0f;
			bcFLOAT m_age = 0;
			core::bc_vector3f m_light_direction;
			bool m_has_started = false;

			bc_scene* m_scene = nullptr;
			core::bc_query_result<bc_scene_query> m_scene_terrain_query;
		};
	}
}