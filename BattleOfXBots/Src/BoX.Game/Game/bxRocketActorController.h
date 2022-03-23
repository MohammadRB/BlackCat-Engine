// [04/27/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Query/bcSceneQuery.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_rocket_controller : public game::bci_actor_controller
	{
	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void added_to_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void update(const game::bc_actor_component_update_content& p_context) override;
		
		const bcCHAR* m_explosion_entity = nullptr;
		bcFLOAT m_speed = 0;

		game::bc_scene* m_scene = nullptr;
		core::bc_vector3f m_position;
		core::bc_vector3f m_direction;
		core::bc_vector3f m_deviated_position;
		core::bc_query_result<game::bc_scene_query> m_scene_query;
	};
}