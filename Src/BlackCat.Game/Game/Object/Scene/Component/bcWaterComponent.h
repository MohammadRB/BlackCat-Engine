// [14/10/2023 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcBaseActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_water_component : public bc_base_actor_component
	{
		BC_COMPONENT(wtr, false, false)

	public:
		bc_water_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_water_component(bc_water_component&& p_other) noexcept = default;

		~bc_water_component() override = default;

		bc_water_component& operator=(bc_water_component&& p_other) noexcept = default;
		
		core::bc_vector3f get_center() const noexcept;

		core::bc_vector2f get_half_extend() const noexcept;

		core::bc_color3 get_color() const noexcept;

		void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;

		void load_instance(const bc_actor_component_load_context& p_context) override;

		void write_instance(const bc_actor_component_write_context& p_context) override;

	private:
		core::bc_vector2f m_half_extend;
		core::bc_color3 m_color;
	};

	inline core::bc_vector3f bc_water_component::get_center() const noexcept
	{
		return get_translation();
	}

	inline core::bc_vector2f bc_water_component::get_half_extend() const noexcept
	{
		return m_half_extend;
	}

	inline core::bc_color3 bc_water_component::get_color() const noexcept
	{
		return m_color;
	}
}
