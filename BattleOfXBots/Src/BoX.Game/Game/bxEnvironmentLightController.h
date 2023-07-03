// [24/04/2022 MRB]

#pragma once

#include "Core/Math/bcVector4f.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_environment_light_controller : public game::bci_actor_controller
	{
	protected:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void update(const game::bc_actor_component_update_content& p_context) override;

	private:
		bcFLOAT m_time_change_per_seconds{};
		bcFLOAT m_sunrise_time{};
		bcFLOAT m_daylight_time{};
		bcFLOAT m_sunset_time{};
		bcFLOAT m_nightlight_time{};

		core::bc_vector3f m_sunrise_direction{};
		core::bc_vector4f m_sunrise_color{};
		core::bc_vector4f m_sunrise_ambient_color{};
		core::bc_vector3f m_daylight_direction{};
		core::bc_vector4f m_daylight_color{};
		core::bc_vector4f m_daylight_ambient_color{};
		core::bc_vector3f m_sunset_direction{};
		core::bc_vector4f m_sunset_color{};
		core::bc_vector4f m_sunset_ambient_color{};
		core::bc_vector3f m_nightlight_direction{};
		core::bc_vector4f m_nightlight_color{};
		core::bc_vector4f m_nightlight_ambient_color{};
		
		game::bc_light_component* m_light_component{ nullptr };
		bcFLOAT m_current_time{};
		core::bc_vector3f m_current_light_direction{};
		core::bc_vector4f m_current_light_color{};
		core::bc_vector4f m_current_ambient_color{};
	};
}
