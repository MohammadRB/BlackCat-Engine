// [12/12/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcOptional.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include <variant>

namespace black_cat::game
{
	enum class bc_wind_type
	{
		direct, point
	};

	struct bc_direct_wind
	{
	public:
		bc_direct_wind(const core::bc_vector3f& p_direction, bcFLOAT p_power)
			: m_direction(p_direction),
			  m_power(p_power)
		{
		}
			
		core::bc_vector3f m_direction;
		bcFLOAT m_power;
	};

	struct bc_point_wind
	{
	public:
		bc_point_wind(const core::bc_vector3f& p_position, bcFLOAT p_radios, bcFLOAT p_power)
			: m_position(p_position),
			  m_radios(p_radios),
			  m_power(p_power)
		{
		}
			
		core::bc_vector3f m_position;
		bcFLOAT m_radios;
		bcFLOAT m_power;
	};

	class BC_GAME_DLL bc_wind
	{
	public:
		explicit bc_wind(const bc_direct_wind& p_direct);

		explicit bc_wind(const bc_point_wind& p_point);

		bc_wind(const bc_wind& p_other) noexcept;

		~bc_wind();

		bc_wind& operator=(const bc_wind& p_other) noexcept;

		void set_transformation(const core::bc_matrix4f& p_transformation) noexcept;
			
		const physics::bc_bound_box& get_bound_box() const noexcept;
			
		bc_direct_wind* as_direct_wind() noexcept;
			
		const bc_direct_wind* as_direct_wind() const noexcept;
			
		bc_point_wind* as_point_wind() noexcept;

		const bc_point_wind* as_point_wind() const noexcept;

	private:
		bc_wind_type _get_type() const;

		void _calculate_bound_box() noexcept;
		
		std::variant<bc_direct_wind, bc_point_wind> m_wind;
		physics::bc_bound_box m_bound_box;
	};
		
	class BC_GAME_DLL bc_wind_component : public bci_actor_component
	{
		BC_COMPONENT(wind, true, false)

	public:
		bc_wind_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_wind_component(bc_wind_component&& p_other) noexcept = default;

		~bc_wind_component() override = default;

		bc_wind_component& operator=(bc_wind_component&& p_other) noexcept = default;

		bc_actor get_actor() const noexcept override;

		const bc_wind* get_wind() const noexcept;

		void initialize(const bc_actor_component_initialize_context& p_context) override;

		void handle_event(const bc_actor_component_event_context& p_context) override;

	private:			
		core::bc_optional<bc_wind> m_wind;
	};
}
