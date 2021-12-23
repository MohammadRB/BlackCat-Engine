// [12/19/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Query/bcSceneQuery.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_explosion_actor_controller : public game::bci_actor_controller
	{
	public:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void update(const game::bc_actor_component_update_content& p_context) override;

	private:
		const bcCHAR* m_emitter_name = nullptr;
		const bcCHAR* m_decal_name = nullptr;
		bcFLOAT m_light_intensity = 0;
		bcFLOAT m_light_particle_intensity = 0;
		bcFLOAT m_light_flare_intensity = 0;
		bcFLOAT m_light_radius = 0;
		bcFLOAT m_light_rise_per_second = 0;
		bcFLOAT m_light_lifetime_second = 1.0f;
		bcFLOAT m_age = 0;
		core::bc_vector3f m_direction;
		bool m_has_started = false;

		game::bc_scene* m_scene = nullptr;
		core::bc_query_result<game::bc_scene_query> m_scene_terrain_query;
	};
}