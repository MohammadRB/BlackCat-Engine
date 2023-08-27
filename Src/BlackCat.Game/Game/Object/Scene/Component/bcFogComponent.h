// [23/07/2023 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class BC_GAME_DLL bc_fog_component : public bci_actor_component
	{
		BC_COMPONENT(fog, false, false)

	public:
		bc_fog_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_fog_component(bc_fog_component&& p_other) noexcept = default;

		~bc_fog_component() override = default;

		bc_fog_component& operator=(bc_fog_component&& p_other) noexcept = default;

		bc_actor get_actor() const noexcept override;

		[[nodiscard]]
		core::bc_vector3f get_center() const noexcept;

		[[nodiscard]]
		const core::bc_vector3f& get_extend() const noexcept;

		[[nodiscard]]
		core::bc_vector3f get_min() const noexcept;

		[[nodiscard]]
		core::bc_vector3f get_max() const noexcept;

		void initialize_entity(const bc_actor_component_initialize_entity_context& p_context) override;

		void load_instance(const bc_actor_component_load_context& p_context) override;

		void write_instance(const bc_actor_component_write_context& p_context) override;
		
	private:
		core::bc_vector3f m_extend;
	};

	inline const core::bc_vector3f& bc_fog_component::get_extend() const noexcept
	{
		return m_extend;
	}

	inline core::bc_vector3f bc_fog_component::get_min() const noexcept
	{
		return { get_center() - get_extend() / 2 };
	}

	inline core::bc_vector3f bc_fog_component::get_max() const noexcept
	{
		return { get_center() + get_extend() / 2 };
	}
}
