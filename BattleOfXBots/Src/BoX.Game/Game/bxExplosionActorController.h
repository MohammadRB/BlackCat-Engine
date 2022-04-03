// [12/19/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Query/bcSceneQuery.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_explosion_actor_controller : public game::bci_actor_controller
	{
	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void update(const game::bc_actor_component_update_content& p_context) override;

		core::bc_query_result<game::bc_scene_query> _build_terrain_query(core::bc_query_manager& p_query_manager, const core::bc_vector3f& p_explosion_position) const;

		core::bc_query_result<game::bc_scene_query> _build_dynamics_query(core::bc_query_manager& p_query_manager, const core::bc_vector3f& p_explosion_position) const;

		bool _test_actor_visibility(const physics::bc_scene& p_px_scene, const game::bc_overlap_hit& p_hit, const core::bc_vector3f& p_explosion_position) const;

		const bcCHAR* m_emitter_name = nullptr;
		const bcCHAR* m_decal_name = nullptr;
		game::bc_network_client_id m_player_id{ game::bc_network_client::invalid_id };
		bcFLOAT m_force_amount = 0;
		bcFLOAT m_force_radius = 0;
		bcFLOAT m_light_intensity = 0;
		bcFLOAT m_light_particle_intensity = 0;
		bcFLOAT m_light_flare_intensity = 0;
		bcFLOAT m_light_radius = 0;
		bcFLOAT m_light_rise_per_second = 0;
		bcFLOAT m_light_lifetime_second = 1.0f;
		bcFLOAT m_sound_lifetime_second = m_light_lifetime_second;
		bcFLOAT m_age = 0;
		core::bc_vector3f m_position;
		core::bc_vector3f m_direction;
		bool m_has_started = false;

		game::bc_scene* m_scene = nullptr;
		game::bc_light_component* m_light_component = nullptr;
		core::bc_query_result<game::bc_scene_query> m_scene_terrain_query;
		core::bc_query_result<game::bc_scene_query> m_scene_dynamics_query;
	};
}